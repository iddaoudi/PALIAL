// ompt_callback_parallel_begin_t type is used for callbacks that are dispatched when parallel and teams regions start
static void trace_ompt_callback_parallel_begin (ompt_data_t *encountering_task_data,
        const ompt_frame_t *encountering_task_frame,
        ompt_data_t *parallel_data,
        unsigned int requested_parallelism,
        int flags,
        const void *codeptr_ra)
{
    unsigned int thread_id = ompt_get_thread_data()->value;
    //  flags argument indicates whether the code for the region is inlined into the application or invoked by the runtime
    //  and also whether the region is a parallel or teams region
    //  ompt_parallel_team indicates that the callback is invoked due to the creation of a team of threads by a parallel construct (wtf)
    if (flags & ompt_parallel_team)
        c_counter[thread_id]._cc.parallel_begin += 1;
    parallel_data->value = ompt_get_unique_id();
}

static void trace_ompt_callback_parallel_end (ompt_data_t *parallel_data,
        ompt_data_t *encountering_task_data,
        int flags,
        const void *codeptr_ra)
{
    unsigned int thread_id = ompt_get_thread_data()->value;
    //unsigned int thread_id = ompt_get_thread_data()->value;
    if (flags & ompt_parallel_team)
        c_counter[thread_id]._cc.parallel_end += 1;
}
