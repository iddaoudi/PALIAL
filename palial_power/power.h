/**
 * File              : power.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 11.05.2022
 * Last Modified Date: 14.07.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <cpufreq.h>

struct cpufreq_available_governors *governor;
struct cpufreq_policy              *policy_governor;
struct cpufreq_policy              *new_policy_governor;

void PALIAL_query_available_governors(unsigned int cpu)
{
    governor = cpufreq_get_available_governors(cpu);
    printf("Available governors: ");
    if (governor)
    {
        while (governor->next)
        {
            printf("%s ", governor->governor);
            governor = governor->next; 
        }
        printf("%s ", governor->governor);
        printf("\n");
    }
    //free memory
    cpufreq_put_available_governors(governor);
}

char* PALIAL_query_current_governor_policy(unsigned int cpu)
{
    policy_governor = cpufreq_get_policy(cpu);
    char *tmp = NULL;
    if (policy_governor)
    {
        strcpy(tmp, policy_governor->governor);
    }
    //free memory
    cpufreq_put_policy(policy_governor);
    return tmp;
}

double PALIAL_query_current_frequency_hardware(unsigned int cpu)
{
    return cpufreq_get_freq_hardware(cpu)*1e-3; //Mhz
}

double PALIAL_query_current_frequency_kernel(unsigned int cpu)
{
    return cpufreq_get_freq_kernel(cpu)*1e-3; //Mhz
}

void PALIAL_set_governor_policy(unsigned int cpu, char* new_governor)
{
    int ret = cpufreq_modify_policy_governor(cpu, new_governor);
    if (ret != 0)
    {
        printf("No root access.\n");
        exit(EXIT_FAILURE);
    }
}

void PALIAL_set_max_frequency(unsigned int cpu, unsigned long max_freq)
{
    int ret = cpufreq_modify_policy_max(cpu, max_freq);
    
    if (ret != 0)
    {
        printf("No root access.\n");
        exit(EXIT_FAILURE);
    }
}
