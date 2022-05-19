/**
 * File              : palial_trace.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 25.04.2022
 * Last Modified Date: 19.05.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#define LOG 1

#include "palial_trace.h"
#include "log.h"

// this upstream function is called everytime before task creation, so the new_name is always ready to be inserted in the task object
extern void trace_palial_set_task_name (const char *name)
{
    new_name = name;
}

// this upstream function is called everytime inside a RUNNING task, therefore the corresponding task object has already been created
extern void trace_palial_set_task_cpu_node (int cpu, int node, char* name)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < cvector_size(ompt_tasks); i++)
    {
        if (strcmp(ompt_tasks[i]->name, name) == 0)
        {
            ompt_tasks[i]->cpu      = cpu;
            ompt_tasks[i]->node     = node;
            ompt_tasks[i]->finished = true;
        }
    }
    pthread_mutex_unlock(&mutex);
}

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

    palial_task_t* task = (palial_task_t*)malloc(sizeof(*task));
    new_task_data->ptr       = task;
    task->id                 = new_task_data->value;
    task->name               = malloc(MAX_STRING_SIZE * sizeof(char));
    strcpy(task->name, new_name);
    task->task_ptr           = codeptr_ra;
    task->n_dependences      = 0;
    task->dependences        = NULL;
    task->start_time         = 0;
    task->end_time           = 0;
    task->scheduled          = false;
    task->finished           = false;
    task->n_task_dependences = 0;
    task->cpu                = 0;
    task->node               = 0;
    pthread_mutex_lock(&mutex);
    cvector_push_back(ompt_tasks, task);
    pthread_mutex_unlock(&mutex);
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
            pthread_mutex_lock(&mutex);
            task->scheduled = true;
            pthread_mutex_unlock(&mutex);
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
    pthread_mutex_lock(&mutex);
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
    pthread_mutex_unlock(&mutex);
}

static void trace_ompt_callback_task_dependence (ompt_data_t *src_task_data,
        ompt_data_t *sink_task_data)
{
    unsigned int thread_id = ompt_get_thread_data()->value;
    c_counter[thread_id]._cc.task_dependence += 1;
    
    pthread_mutex_lock(&mutex);
    palial_task_t *src_task  = NULL;
    palial_task_t *sink_task = NULL;
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
    if (sink_task != NULL && src_task != NULL)
    {
        int count = sink_task->n_task_dependences;
        sink_task->task_dependences[count] = src_task->id;
        sink_task->n_task_dependences += 1;
    }
    pthread_mutex_unlock(&mutex);
}

int ompt_initialize (ompt_function_lookup_t lookup,
        int initial_device_num,
        ompt_data_t *data_from_tool)
{
    // Runtime entrypoint
    ompt_set_callback_t ompt_set_callback =(ompt_set_callback_t) lookup("ompt_set_callback");
    ompt_get_thread_data = (ompt_get_thread_data_t) lookup("ompt_get_thread_data");
    ompt_get_unique_id   = (ompt_get_unique_id_t) lookup("ompt_get_unique_id");
    ompt_get_proc_id     = (ompt_get_proc_id_t) lookup("ompt_get_proc_id");

    // Callbacks registering
    register_callback(ompt_callback_task_create);
    //register_callback(ompt_callback_task_schedule);
    register_callback(ompt_callback_dependences);
    register_callback(ompt_callback_task_dependence);

    c_counter = calloc(MAX_THREADS, sizeof(callback_counter_type));
    data_from_tool->ptr = c_counter;

    return 1;
}

void ompt_finalize (ompt_data_t *data_from_tool)
{
    sum_of_callbacks(data_from_tool->ptr);
#ifdef LOG
    palial_log_trace(ompt_tasks);
#endif
    for (int i = 0; i < cvector_size(ompt_tasks); i++)
    {
        free(ompt_tasks[i]->dependences);
        free(ompt_tasks[i]->name);
        free(ompt_tasks[i]);
    }
    free(data_from_tool->ptr);
    pthread_mutex_destroy(&mutex);
}

ompt_start_tool_result_t *ompt_start_tool (unsigned int omp_version, const char *runtime_version)
{
    static ompt_start_tool_result_t ompt_start_tool_result = {&ompt_initialize, &ompt_finalize, {.value = 0}};
    return &ompt_start_tool_result;
}
