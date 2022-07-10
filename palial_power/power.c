/**
 * File              : power.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 11.05.2022
 * Last Modified Date: 11.05.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <cpufreq.h>

#define ITER 20

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

void PALIAL_query_current_governor_policy(unsigned int cpu)
{
    policy_governor = cpufreq_get_policy(cpu);
    printf("Current governor policy: ");
    if (policy_governor)
    {
        printf("%s\n", policy_governor->governor);
    }
    //free memory
    cpufreq_put_policy(policy_governor);
}

void PALIAL_query_current_frequency_hardware(unsigned int cpu)
{
    printf("Current frequency (hardware) of core %d: %f Mhz\n", cpu, cpufreq_get_freq_hardware(cpu)*1e-3);
}

void PALIAL_query_current_frequency_kernel(unsigned int cpu)
{
    printf("Current frequency (kernel) of core %d: %f Mhz\n", cpu, cpufreq_get_freq_kernel(cpu)*1e-3);
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

int main ()
{
    //int ret = setreuid(0, 0);
    //printf("ret : %d\n", ret);
    
    unsigned int cpu_number = 0;
    unsigned int cpu_number2 = 1;

    struct timespec time;
    time.tv_sec = 0L;
    time.tv_nsec = 500*1e6L;

    for (int i = 0; i < ITER; i++)
    {
        PALIAL_query_current_frequency_kernel(cpu_number);
        PALIAL_query_current_frequency_kernel(cpu_number2);
        PALIAL_query_current_frequency_hardware(cpu_number);
        PALIAL_query_current_frequency_hardware(cpu_number2);
        nanosleep(&time, &time);
    }

    PALIAL_query_current_governor_policy(cpu_number);
    PALIAL_query_available_governors(cpu_number); 
    
    PALIAL_set_governor_policy(cpu_number, "userspace");
    PALIAL_set_governor_policy(cpu_number2, "userspace");
    
    PALIAL_query_current_governor_policy(cpu_number);
    PALIAL_query_current_governor_policy(cpu_number2);
    
    PALIAL_set_max_frequency(cpu_number, 800000);
    PALIAL_set_max_frequency(cpu_number2, 3000000);
    for (int i = 0; i < ITER; i++)
    {
        PALIAL_query_current_frequency_kernel(cpu_number);
        PALIAL_query_current_frequency_kernel(cpu_number2);
        PALIAL_query_current_frequency_hardware(cpu_number);
        PALIAL_query_current_frequency_hardware(cpu_number2);
        nanosleep(&time, &time);
    }
    PALIAL_set_max_frequency(cpu_number, 3200000);
    PALIAL_set_max_frequency(cpu_number2, 3200000);
    
    
    
    
    
    
    PALIAL_set_governor_policy(cpu_number, "ondemand");
    PALIAL_set_governor_policy(cpu_number2, "ondemand");
    return 0;
}
