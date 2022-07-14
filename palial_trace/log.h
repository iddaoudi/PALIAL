/**
 * File              : log.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 02.05.2022
 * Last Modified Date: 14.07.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

void palial_log_trace (cvector_vector_type(palial_task_t*) ompt_tasks)
{
    for (int i = 0; i < cvector_size(ompt_tasks); i++)
    {
        printf("Task ID          : %ld\n", ompt_tasks[i]->id);
        printf("Task name        : %s\n", ompt_tasks[i]->name);
        printf("Task ndeps       : %d\n", ompt_tasks[i]->n_dependences);
        printf("Task access mode : ");
        for (int j = 0; j < ompt_tasks[i]->n_dependences; j++)
        {
            printf("%s ", ompt_tasks[i]->access_mode[j]);
        }
        printf("\n");
        printf("Task scheduled   : %d\n", ompt_tasks[i]->scheduled);
        printf("Task start time  : %f\n", ompt_tasks[i]->start_time);
        printf("Task end time    : %f\n", ompt_tasks[i]->end_time);
        printf("Task cpu         : %d\n", ompt_tasks[i]->cpu);
        printf("Task node        : %d\n", ompt_tasks[i]->node);
        printf("Task dependences : ");
        for (int j = 0; j < ompt_tasks[i]->n_task_dependences; j++)
        {
            for (int k = 0; k < cvector_size(ompt_tasks); k++)
            {
                if (ompt_tasks[i]->task_dependences[j] == ompt_tasks[k]->id)
                {
                    printf("%s ", ompt_tasks[k]->name);
                }
            }
        }
        printf("\n");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }
}
