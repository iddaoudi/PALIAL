// ompt_callback_thread_begin is used for callbacks that are dispatched when native threads are created;
// therefore, the number of ompt_callback_thread_begin is always equal to the value of OMP_NUM_THREADS
static void trace_ompt_callback_thread_begin (ompt_thread_t thread_type,
                                              ompt_data_t *thread_data)
{
    unsigned int thread_id = thread_data->value = unique_thread_id();
    c_counter[thread_id]._cc.thread_begin += 1;
}

static void trace_ompt_callback_thread_end (ompt_data_t *thread_data)
{
    unsigned int thread_id = thread_data->value;
    c_counter[thread_id]._cc.thread_end += 1;
}
