#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include <pthread.h>
#include <omp.h>
#include <omp-tools.h>
#include "../include/cvector.h"

#define MAX_THREADS     128

#include "ompt_callbacks.h"

#define MAX_STRING_SIZE 64
#define MAX_TASKS       1000000 /* A million tasks is enough for now, FIXME tho with some juicy realloc */
#define MAX_MODES       128

pthread_mutex_t mutex;

static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_unique_id_t   ompt_get_unique_id;
static ompt_get_proc_id_t     ompt_get_proc_id;
static ompt_get_place_num_t   ompt_get_place_num;

callback_counter_type *c_counter;

const char *new_name        = NULL;

extern void trace_palial_set_task_name (const char *name);
extern void trace_palial_set_task_cpu_node (int cpu, int node, char* name);

extern void palial_set_task_name (const char *name)
{
    trace_palial_set_task_name(name);
}

extern void palial_set_task_cpu_node (int cpu, int node, char* name)
{
    trace_palial_set_task_cpu_node(cpu, node, name);
}

typedef struct {
   const void*        task_ptr;
   char*              name;
   uint64_t           id;
   int                n_dependences;
   ompt_dependence_t* dependences;
   char               access_mode[MAX_MODES][MAX_STRING_SIZE];
   double             start_time;
   double             end_time;
   bool               scheduled;
   bool               finished;
   int                n_task_dependences;
   int                task_dependences[MAX_MODES];
   int                cpu;
   int                node;
} palial_task_t;

cvector_vector_type(palial_task_t*) ompt_tasks = NULL;

static void trace_ompt_callback_task_create (ompt_data_t *encountering_task_data,
      const ompt_frame_t *encountering_task_frame,
      ompt_data_t *new_task_data,
      int flags,
      int has_dependences,
      const void *codeptr_ra);

static void trace_ompt_callback_task_schedule (ompt_data_t *prior_task_data,
      ompt_task_status_t prior_task_status,
      ompt_data_t *next_task_data);

static void trace_ompt_callback_dependences (ompt_data_t *task_data,
      const ompt_dependence_t *deps,
      int ndeps);

static void trace_ompt_callback_task_dependence (ompt_data_t *src_task_data,
      ompt_data_t *sink_task_data);

int ompt_initialize (ompt_function_lookup_t lookup,
      int initial_device_num,
      ompt_data_t *data_from_tool);

void ompt_finalize (ompt_data_t *data_from_tool);

ompt_start_tool_result_t *ompt_start_tool (unsigned int omp_version, const char *runtime_version);
