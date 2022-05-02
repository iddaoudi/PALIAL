#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include <omp.h>
#include <omp-tools.h>
#include "../src/cvector.h"

#define MAX_THREADS     128
#define MAX_MODES       50
#define MAX_STRING_SIZE 8

#include "ompt_callbacks.h"

static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_unique_id_t   ompt_get_unique_id;
static ompt_get_proc_id_t     ompt_get_proc_id;
static ompt_get_place_num_t   ompt_get_place_num;

callback_counter_type *c_counter;
int counter = 0;

extern void trace_palial_set_task_name (const char *name);
extern void __trace_palial_set_task_name (const char *name)
{
    trace_palial_set_task_name(name);
}
extern void palial_set_task_name (const char *name)
{
    trace_palial_set_task_name(name);
}

typedef struct {
   const void*        task_ptr;
   const char*        name;
   uint64_t           id;
   int                n_dependences;
   ompt_dependence_t* dependences;
   char               access_mode[MAX_MODES][MAX_STRING_SIZE];
   double             start_time;
   double             end_time;
   bool               scheduled; // if = 1, task is completed
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
