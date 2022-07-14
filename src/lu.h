/**
 * File              : lu.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 03.02.2022
 * Last Modified Date: 14.07.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "srclu/dgetrf.h"

void lu (MATRIX_desc A)
{
    int k, n, m;
    for (k = 0; k < A.matrix_size/A.tile_size; k++)
    {
        double *tileA = A(k,k);

        char name_with_id_char[32];
        strncpy(name_with_id_char, "getrf", 32);
        char k_to_str[8], m_to_str[8], n_to_str[8];
        sprintf(k_to_str, "%d", k);
        sprintf(m_to_str, "%d", m);
        sprintf(n_to_str, "%d", n);
        strcat(name_with_id_char, k_to_str);
        strcat(name_with_id_char, m_to_str);
        strcat(name_with_id_char, n_to_str);

        upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(inout : tileA[0])
        {
            struct timeval start, end;
            
            gettimeofday(&start, NULL);
            PALIAL_dgetrf(A.tile_size,
                    tileA,
                    A.tile_size);
            gettimeofday(&end, NULL);
            
            unsigned int cpu, node;
            getcpu(&cpu, &node);
            upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
            upstream_palial_get_task_time(start, end, name_with_id_char);
        }
        for (m = k+1; m < A.matrix_size/A.tile_size; m++)
        {
            double *tileA = A(k,k);
            double *tileB = A(m,k);

            char name_with_id_char[32];
            strncpy(name_with_id_char, "trsm", 32);
            char k_to_str[8], m_to_str[8], n_to_str[8];
            sprintf(k_to_str, "%d", k);
            sprintf(m_to_str, "%d", m);
            sprintf(n_to_str, "%d", n);
            strcat(name_with_id_char, k_to_str);
            strcat(name_with_id_char, m_to_str);
            strcat(name_with_id_char, n_to_str);

            upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[A.tile_size*A.tile_size])
            {
                struct timeval start, end;
                
                gettimeofday(&start, NULL);
                cblas_dtrsm(CblasColMajor,
                        CblasRight,
                        CblasUpper,
                        CblasNoTrans,
                        CblasNonUnit,
                        A.tile_size,
                        A.tile_size,
                        1.0,
                        tileA,
                        A.tile_size,
                        tileB,
                        A.tile_size);
                gettimeofday(&end, NULL);
                
                unsigned int cpu, node;
                getcpu(&cpu, &node);
                upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
                upstream_palial_get_task_time(start, end, name_with_id_char);
            }
        }
        for (n = k + 1; n < A.matrix_size/A.tile_size; n++)
        {
            double *tileA = A(k,k);
            double *tileB = A(k,n);

            char name_with_id_char[32];
            strncpy(name_with_id_char, "trsm", 32);
            char k_to_str[8], m_to_str[8], n_to_str[8];
            sprintf(k_to_str, "%d", k);
            sprintf(m_to_str, "%d", m);
            sprintf(n_to_str, "%d", n);
            strcat(name_with_id_char, k_to_str);
            strcat(name_with_id_char, m_to_str);
            strcat(name_with_id_char, n_to_str);

            upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[A.tile_size*A.tile_size])
            {
                struct timeval start, end;
                
                gettimeofday(&start, NULL);
                cblas_dtrsm(CblasColMajor,
                        CblasLeft,
                        CblasLower,
                        CblasNoTrans,
                        CblasUnit,
                        A.tile_size,
                        A.tile_size,
                        1.0,
                        tileA,
                        A.tile_size,
                        tileB,
                        A.tile_size);
                gettimeofday(&end, NULL);
                
                unsigned int cpu, node;
                getcpu(&cpu, &node);   
                upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
                upstream_palial_get_task_time(start, end, name_with_id_char);
            }
            for (m = k + 1; m < A.matrix_size/A.tile_size; m++)
            {
                double *tileA = A(m,k);
                double *tileB = A(k,n);
                double *tileC = A(m,n);

                char name_with_id_char[32];
                strncpy(name_with_id_char, "gemm", 32);
                char k_to_str[8], m_to_str[8], n_to_str[8];
                sprintf(k_to_str, "%d", k);
                sprintf(m_to_str, "%d", m);
                sprintf(n_to_str, "%d", n);
                strcat(name_with_id_char, k_to_str);
                strcat(name_with_id_char, m_to_str);
                strcat(name_with_id_char, n_to_str);

                upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(in : tileA[0:A.tile_size*A.tile_size], tileB[0:A.tile_size*A.tile_size]) depend(inout : tileC[0:A.tile_size*A.tile_size])
                {
                    struct timeval start, end;
                    
                    gettimeofday(&start, NULL);
                    cblas_dgemm(CblasColMajor,
                            CblasNoTrans,
                            CblasNoTrans,
                            A.tile_size,
                            A.tile_size,
                            A.tile_size,
                            -1.0,
                            tileA,
                            A.tile_size,
                            tileB,
                            A.tile_size,
                            1.0,
                            tileC,
                            A.tile_size);
                    gettimeofday(&end, NULL);
                    
                    unsigned int cpu, node;
                    getcpu(&cpu, &node);
                    upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
                    upstream_palial_get_task_time(start, end, name_with_id_char);
                }
            }
        }
    }
}
