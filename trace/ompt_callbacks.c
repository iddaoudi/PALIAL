
void sum_of_callbacks (callback_counter_type *c_counter)
{
    for (int i = 0; i < MAX_THREADS, i++)
    {
        c_counter[0]._cc.parallel_begin += c_counter[i]._cc.parallel_begin;
        c_counter[0]._cc.parallel_end   += c_counter[i]._cc.parallel_end;
    }
    printf("Total parallel_begin callbacks: %d\n", c_counter[0]._cc.parallel_begin);
    printf("Total parallel_end callbacks: %d\n", c_counter[0]._cc.parallel_end);
    printf("Total registered callbacks: %d\n", c_counter[0]._cc.parallel_begin + c_counter[0]._cc.parallel_end);
}
