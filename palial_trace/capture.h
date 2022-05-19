void capture ()
{
    volatile bool condition = false;
    while (condition == false)
    {
        for (int i = 0; i < cvector_size(ompt_tasks); i++)
        {
            if (strcmp(ompt_tasks[i]->name, "syrk233") == 0)
            {
                if (ompt_tasks[i]->finished == false)
                    printf("task is still running\n");
                else
                    condition = true;
            }
        }    
    }
    printf("task has ended ducon\n");
}
