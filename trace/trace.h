#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#define _GNU_SOURCE

#include <omp.h>
#include <omp-tools.h>
#include "../src/cvector.h"

#define MAX_THREADS     128
#define MAX_MODES       5
#define MAX_STRING_SIZE 8

#include "ompt_callbacks.h"

static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_unique_id_t   ompt_get_unique_id;
static ompt_get_proc_id_t     ompt_get_proc_id;
callback_counter_type *c_counter;
int counter = 0;

#include "thread_id.h"
#include "thread_beginend.h"
#include "parallel_beginend.h"

typedef struct {
    const void        *task_ptr;
    const char*       name;
    uint64_t          id;
    int               n_dependences;
    ompt_dependence_t *dependences;
    char              access_mode[MAX_MODES][MAX_STRING_SIZE];
    double            start_time;
    double            end_time;
    bool              scheduled; // if = 1, task is completed
    int               node;
    int               task_dependences;
} palial_task_t;

cvector_vector_type(palial_task_t*) ompt_tasks = NULL;

// ompt_callback_task_create is used for callbacks that are dispatched when task regions or initial tasks are generated
static void trace_ompt_callback_task_create (ompt_data_t *encountering_task_data,
        const ompt_frame_t *encountering_task_frame,
        ompt_data_t *new_task_data,
        int flags,
        int has_dependences,
        const void *codeptr_ra)
{
    unsigned int thread_id = ompt_get_thread_data()->value;
    // all tasks except first and last ones
    if (flags & ompt_task_explicit)
        c_counter[thread_id]._cc.task_create_explicit += 1;
    // final task
    if (flags & ompt_task_final)
        c_counter[thread_id]._cc.task_create_finalize += 1;
    new_task_data->value = ompt_get_unique_id();
    
    palial_task_t *task = (palial_task_t*)malloc(sizeof(palial_task_t));
    new_task_data->ptr = task;
    task->id = new_task_data->value;
    if (has_dependences != 0)
    {
        if (cvector_size(ompt_task_names) != 0)
            task->name = ompt_task_names[counter];
        counter++;
    }
    task->task_ptr = codeptr_ra;
    cvector_push_back(ompt_tasks, task);
    unsigned int cpu, node;
    getcpu(&cpu, &node);
    task->node = node;
}

// ompt_callback_task_schedule is used for callbacks that are dispatched when task scheduling decisions are made
static void trace_ompt_callback_task_schedule (ompt_data_t *prior_task_data,
        ompt_task_status_t prior_task_status,
        ompt_data_t *next_task_data)
{
    unsigned int thread_id = ompt_get_thread_data()->value;
    c_counter[thread_id]._cc.task_schedule += 1;
    palial_task_t *task;
    for (int i = 0; i < cvector_size(ompt_tasks); i++)
    {
        if (ompt_tasks[i]->id == prior_task_data->value)
        {
            task = ompt_tasks[i];
            task->scheduled = true;
            break;
        }
    }
}

// ompt_callback_dependences is used for callbacks that are related to dependences and that are dispatched when new tasks are generated
static void trace_ompt_callback_dependences (ompt_data_t *task_data,
        const ompt_dependence_t *deps,
        int ndeps)
{
    unsigned int thread_id = ompt_get_thread_data()->value;
    c_counter[thread_id]._cc.dependences += 1;
    palial_task_t *task;
    // Find the task
    for (int i = 0; i < cvector_size(ompt_tasks); i++)
    {
        if (ompt_tasks[i]->id == task_data->value)
        {
            task = ompt_tasks[i];
            break;
        }
    }
    task->dependences = (ompt_dependence_t*)malloc(ndeps * sizeof(ompt_dependence_t));
    task->n_dependences = ndeps;
    memcpy(task->dependences, deps, ndeps * sizeof(ompt_dependence_t));
    assert(ndeps < MAX_MODES);
    for (int i = 0; i < ndeps; i++)
    {
        if (task->dependences[i].dependence_type == 1)
            strcpy(task->access_mode[i], "R");
        else if (task->dependences[i].dependence_type == 2)
            strcpy(task->access_mode[i], "W");
        else if (task->dependences[i].dependence_type == 3)
            strcpy(task->access_mode[i], "RW");
        else
        {
            printf("Unsupported access mode. Aborting...\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void trace_ompt_callback_task_dependence (ompt_data_t *src_task_data,
        ompt_data_t *sink_task_data)
{
    printf("Called task_dependences\n");
    unsigned int thread_id = ompt_get_thread_data()->value;
    c_counter[thread_id]._cc.task_dependence += 1;
    printf("hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
    palial_task_t *src_task, *sink_task;
    // Find the task
    for (int i = 0; i < cvector_size(ompt_tasks); i++)
    {
        if (ompt_tasks[i]->id == src_task_data->value)
        {
            src_task = ompt_tasks[i];
            break;
        }
    }
    for (int i = 0; i < cvector_size(ompt_tasks); i++)
    {
        if (ompt_tasks[i]->id == sink_task_data->value)
        {
            sink_task = ompt_tasks[i];
            break;
        }
    }
    printf("found src : name: %s\n", src_task->name);
    printf("found sink: name: %s\n", sink_task->name);
}

int ompt_initialize (ompt_function_lookup_t lookup,
        int initial_device_num,
        ompt_data_t *data_from_tool)
{
    //printf("Tool initialized.\n");
    // Runtime entrypoint
    ompt_set_callback_t ompt_set_callback =(ompt_set_callback_t) lookup("ompt_set_callback");
    ompt_get_thread_data = (ompt_get_thread_data_t) lookup("ompt_get_thread_data");
    ompt_get_unique_id   = (ompt_get_unique_id_t) lookup("ompt_get_unique_id");
    ompt_get_proc_id     = (ompt_get_proc_id_t) lookup("ompt_get_proc_id");

    // Callbacks registering
    register_callback(ompt_callback_task_create);
    register_callback(ompt_callback_task_dependence);
    register_callback(ompt_callback_task_schedule);
    register_callback(ompt_callback_dependences);
    
    //ompt_callback_t f_ompt_callback_dependences = &trace_ompt_callback_dependences;
    //info = ompt_set_callback(ompt_callback_dependences, f_ompt_callback_dependences);
    //printf("Dependences: %d\n", info);

    c_counter = calloc(MAX_THREADS, sizeof(callback_counter_type));
    data_from_tool->ptr = c_counter;

    //printf("Init time: %f\n", omp_get_wtime() - *(double*)(data_from_tool->ptr));
    //*(double*)(data_from_tool->ptr) = omp_get_wtime();
    return 1;
}

void ompt_finalize (ompt_data_t *data_from_tool)
{
    sum_of_callbacks(data_from_tool->ptr);
    //printf("Runtime: %f\n", omp_get_wtime() - *(double*)(data_from_tool->ptr));
    //printf("Tool finalized.\n");
    free(data_from_tool->ptr);
}

ompt_start_tool_result_t *ompt_start_tool (unsigned int omp_version, const char *runtime_version)
{
    static double time = 0;
    time = omp_get_wtime();
    static ompt_start_tool_result_t ompt_start_tool_result = {&ompt_initialize, &ompt_finalize, {.value = 0}};
    return &ompt_start_tool_result;
}
