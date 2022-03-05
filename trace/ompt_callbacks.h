#include <sys/resource.h>
#include <stdio.h>

#define CACHE_LINE 128

// using concatenation to convert callback functions names
#define register_callback_t(name, type)                                            \
    do {                                                                           \
        type f_##name = &trace_##name;                                             \
        if (ompt_set_callback(name, (ompt_callback_t)f_##name) == ompt_set_never)  \
            printf("Callback '"#name"' could not be registered.\n");               \
    } while(0)

#define register_callback(name) register_callback_t(name, name##_t)

typedef struct
{
    int parallel_begin;
    int parallel_end;
    int thread_begin;
    int thread_end;
    int task_create_initialize;
    int task_create_implicit;
    int task_create_explicit;
    int task_create_finalize;
    int task_schedule;
    int task_dependence;
    int implicit_task_scope_begin;
    int implicit_task_scope_end;
    int initial_task_scope_begin;
    int initial_task_scope_end;
} callback_counter;

typedef union __attribute__((aligned(CACHE_LINE))) callback_counter_union
{
    callback_counter _cc;
} callback_counter_type;

void sum_of_callbacks (callback_counter_type *c_counter)
{
    for (int i = 0; i < MAX_THREADS; i++)
    {
        c_counter[0]._cc.parallel_begin            += c_counter[i]._cc.parallel_begin;
        c_counter[0]._cc.parallel_end              += c_counter[i]._cc.parallel_end;
        c_counter[0]._cc.thread_begin              += c_counter[i]._cc.thread_begin;
        c_counter[0]._cc.thread_end                += c_counter[i]._cc.thread_end;
        
        c_counter[0]._cc.initial_task_scope_begin  += c_counter[i]._cc.initial_task_scope_begin;
        c_counter[0]._cc.initial_task_scope_end    += c_counter[i]._cc.initial_task_scope_end;
        c_counter[0]._cc.implicit_task_scope_begin += c_counter[i]._cc.implicit_task_scope_begin;
        c_counter[0]._cc.implicit_task_scope_end   += c_counter[i]._cc.implicit_task_scope_end;
        c_counter[0]._cc.task_create_explicit      += c_counter[i]._cc.task_create_explicit;
        c_counter[0]._cc.task_create_finalize      += c_counter[i]._cc.task_create_finalize;
        c_counter[0]._cc.task_schedule             += c_counter[i]._cc.task_schedule;
        c_counter[0]._cc.task_dependence           += c_counter[i]._cc.task_dependence;
    }
//    printf("Total parallel_begin callbacks: %d\n", c_counter[0]._cc.parallel_begin);
//    printf("Total thread_begin callbacks  : %d\n", c_counter[0]._cc.thread_begin);
    
//    printf("Total initial_task_begin      : %d\n", c_counter[0]._cc.initial_task_scope_begin);
//    printf("Total initial_task_end        : %d\n", c_counter[0]._cc.initial_task_scope_end);
//    printf("Total implicit_task_begin     : %d\n", c_counter[0]._cc.implicit_task_scope_begin);
//    printf("Total implicit_task_end       : %d\n", c_counter[0]._cc.implicit_task_scope_end);
   
//    printf("Total task_create callbacks   : %d\n", c_counter[0]._cc.task_create_explicit);
//    printf("Total task_finalize callbacks : %d\n", c_counter[0]._cc.task_create_finalize);

//    printf("Total task_schedule callbacks : %d\n", c_counter[0]._cc.task_schedule);
    printf("Total task_dependence callbacks : %d\n", c_counter[0]._cc.task_dependence);

//    printf("Total parallel_end callbacks  : %d\n", c_counter[0]._cc.parallel_end);
//    printf("Total thread_end callbacks    : %d\n", c_counter[0]._cc.thread_end);
    
//    printf("Total registered callbacks    : %d\n", c_counter[0]._cc.parallel_begin +
//                                               c_counter[0]._cc.parallel_end +
//                                               c_counter[0]._cc.thread_begin +
//                                               c_counter[0]._cc.thread_end +
//                                               c_counter[0]._cc.initial_task_scope_begin +
//                                               c_counter[0]._cc.initial_task_scope_end +
//                                               c_counter[0]._cc.implicit_task_scope_begin +
//                                               c_counter[0]._cc.implicit_task_scope_end +
//                                               c_counter[0]._cc.task_create_explicit +
//                                               c_counter[0]._cc.task_create_finalize +
//                                               c_counter[0]._cc.task_schedule
//                                               );
}
