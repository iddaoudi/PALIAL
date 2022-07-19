/**
 * File              : server.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 16.07.2022
 * Last Modified Date: 16.07.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#include "power.h"

#define NUMBER_OF_TASKS 4
#define TASK_MAX_STRING_SIZE 5

#define MAX_CPUS 4

char cholesky_tasks[NUMBER_OF_TASKS][TASK_MAX_STRING_SIZE] = {"gemm", "potrf", "syrk", "trsm"};
char qr_tasks[NUMBER_OF_TASKS][TASK_MAX_STRING_SIZE] = {"ormqr", "tsqrt", "tsmqr", "geqrt"};
char lu_tasks[NUMBER_OF_TASKS][TASK_MAX_STRING_SIZE] = {"geqrf", "trsm", "gemm"};

int main ()
{
    // change the governor at the start of the server 
    for (int i = 0; i < MAX_CPUS; i++)
        PALIAL_set_governor_policy(i, "userspace");

    void *context = zmq_ctx_new();
    void *server  = zmq_socket(context, ZMQ_PULL);

    int num = 0;
    zmq_setsockopt(server, ZMQ_LINGER, &num, sizeof(int));
    num = -1;
    zmq_setsockopt(server, ZMQ_SNDHWM, &num, sizeof(int));
    zmq_setsockopt(server, ZMQ_RCVHWM, &num, sizeof(int));

    int ret = zmq_bind(server, "tcp://127.0.0.1:5555");
    assert(ret == 0);

    while(1)
    {
        char task_and_cpu[9];
        zmq_recv(server, task_and_cpu, 9, 0);
        if (strcmp(task_and_cpu, "end") == 0)
            break;

        char task[6];
        int cpu;
        sscanf(task_and_cpu, "%s %d", task, &cpu);
        
        if (strcmp(task, "gemm") == 0)
        {
                        
        }
        else if (strcmp(task, "trsm") == 0)
        {

        }
        else if (strcmp(task, "syrk") == 0)
        {

        }
        else if (strcmp(task, "potrf") == 0)
        {

        }

        //PALIAL_query_current_frequency_kernel(itask_and_cpu);
    }
    
    // set back the original governor policy
    for (int i = 0; i < MAX_CPUS; i++)
        PALIAL_set_governor_policy(i, "ondemand");

    return 0;
}
