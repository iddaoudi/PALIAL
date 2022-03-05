static unsigned int unique_thread_id ()
{
    static unsigned int id = 0;
    unsigned int result = __sync_fetch_and_add(&id, 1);
    if (result > MAX_THREADS)
    {
        printf("Max number of threads reached.\n");
        exit(EXIT_FAILURE);
    }
    return result;
}
