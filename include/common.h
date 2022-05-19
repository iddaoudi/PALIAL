/**
 * File              : common.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 02.05.2022
 * Last Modified Date: 02.05.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

void __attribute__((weak)) palial_set_task_name (const char *name);
void __attribute__((weak)) palial_set_task_cpu_node (int cpu, int node, char* name);

extern inline void upstream_palial_set_task_name (const char *name)
{
    return palial_set_task_name(name);
}

extern inline void upstream_palial_set_task_cpu_node (int cpu, int node, char* name)
{
    palial_set_task_cpu_node(cpu, node, name);
}
