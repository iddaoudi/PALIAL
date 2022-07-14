/**
 * File              : qr.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 01.02.2022
 * Last Modified Date: 14.07.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "common.h"
#include "srcqr/dtsmqr.h"
#include "srcqr/dtsqrt.h"
#include "srcqr/dgeqrt.h"
#include "srcqr/dormqr.h"

void qr(MATRIX_desc A, MATRIX_desc S)
{
    int k, n, m;
    for (k = 0; k < A.matrix_size/A.tile_size; k++)
    {
        double *tileA = A(k,k);
        double *tileS = S(k,k);

        char name_with_id_char[32];
        strncpy(name_with_id_char, "geqrt", 32);
        char k_to_str[8], m_to_str[8], n_to_str[8];
        sprintf(k_to_str, "%d", k);
        sprintf(m_to_str, "%d", m);
        sprintf(n_to_str, "%d", n);
        strcat(name_with_id_char, k_to_str);
        strcat(name_with_id_char, m_to_str);
        strcat(name_with_id_char, n_to_str);

        upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(inout : tileA[0:S.tile_size*S.tile_size]) depend(out : tileS[0:A.tile_size*S.tile_size])
        {
            double tho[S.tile_size];
            double work[S.tile_size * S.tile_size];
            struct timeval start, end;
            
            gettimeofday(&start, NULL);
            PALIAL_dgeqrt(A.tile_size,
                    S.tile_size,
                    tileA,
                    A.tile_size,
                    tileS,
                    S.tile_size,
                    &tho[0],
                    &work[0]);
            gettimeofday(&end, NULL);
            
            unsigned int cpu, node;
            getcpu(&cpu, &node);
            upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
            upstream_palial_get_task_time(start, end, name_with_id_char);
        }
        for (n = k+1; n < A.matrix_size/A.tile_size; n++)
        {
            double *tileA = A(k,k);
            double *tileS = S(k,k);
            double *tileB = A(k,n);

            char name_with_id_char[32];
            strncpy(name_with_id_char, "ormqr", 32);
            char k_to_str[8], m_to_str[8], n_to_str[8];
            sprintf(k_to_str, "%d", k);
            sprintf(m_to_str, "%d", m);
            sprintf(n_to_str, "%d", n);
            strcat(name_with_id_char, k_to_str);
            strcat(name_with_id_char, m_to_str);
            strcat(name_with_id_char, n_to_str);

            upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(in : tileA[0:S.tile_size*S.tile_size], tileS[0:A.tile_size*S.tile_size]) depend(inout : tileB[0:S.tile_size*S.tile_size])
            {
                double work[S.tile_size * S.tile_size];
                struct timeval start, end;
                
                gettimeofday(&start, NULL);
                PALIAL_dormqr(PALIAL_left,
                        PALIAL_transpose,
                        A.tile_size,
                        tileA,
                        A.tile_size,
                        tileS,
                        S.tile_size,
                        tileB,
                        A.tile_size,
                        &work[0],
                        S.tile_size);
                gettimeofday(&end, NULL);
                
                unsigned int cpu, node;
                getcpu(&cpu, &node);
                upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
                upstream_palial_get_task_time(start, end, name_with_id_char);
            }
        }
        for (m = k+1; m < A.matrix_size/A.tile_size; m++)
        {
            double *tileA = A(k,k);
            double *tileS = S(m,k);
            double *tileB = A(m,k);

            char name_with_id_char[32];
            strncpy(name_with_id_char, "tsqrt", 32);
            char k_to_str[8], m_to_str[8], n_to_str[8];
            sprintf(k_to_str, "%d", k);
            sprintf(m_to_str, "%d", m);
            sprintf(n_to_str, "%d", n);
            strcat(name_with_id_char, k_to_str);
            strcat(name_with_id_char, m_to_str);
            strcat(name_with_id_char, n_to_str);

            upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(inout : tileA[0:S.tile_size*S.tile_size], tileB[0:S.tile_size*S.tile_size]) depend(out : tileS[0:S.tile_size*A.tile_size])
            {   
                double work[S.tile_size * S.tile_size];
                double tho[S.tile_size];
                struct timeval start, end;
                
                gettimeofday(&start, NULL);
                PALIAL_dtsqrt(A.tile_size,
                        tileA,
                        A.tile_size,
                        tileB,
                        A.tile_size,
                        tileS,
                        S.tile_size,
                        &tho[0],
                        &work[0]);
                gettimeofday(&end, NULL);
                
                unsigned int cpu, node;
                getcpu(&cpu, &node);   
                upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
                upstream_palial_get_task_time(start, end, name_with_id_char);
            }
            for (n = k+1; n < A.matrix_size/A.tile_size; n++)
            {
                double *tileA = A(k,n);
                double *tileS = S(m,k);
                double *tileB = A(m,n);
                double *tileC = A(m,k);

                char name_with_id_char[32];
                strncpy(name_with_id_char, "tsmqr", 32);
                char k_to_str[8], m_to_str[8], n_to_str[8];
                sprintf(k_to_str, "%d", k);
                sprintf(m_to_str, "%d", m);
                sprintf(n_to_str, "%d", n);
                strcat(name_with_id_char, k_to_str);
                strcat(name_with_id_char, m_to_str);
                strcat(name_with_id_char, n_to_str);

                upstream_palial_set_task_name(name_with_id_char);

#pragma omp task depend(inout : tileA[0:S.tile_size*S.tile_size], tileB[0:S.tile_size*S.tile_size]) depend(in : tileC[0:S.tile_size*S.tile_size], tileS[0:A.tile_size*S.tile_size])
                {
                    double work[S.tile_size * S.tile_size];
                    struct timeval start, end;
                    
                    gettimeofday(&start, NULL);
                    PALIAL_dtsmqr(PALIAL_left,
                            PALIAL_transpose,
                            A.tile_size,
                            A.tile_size,
                            A.tile_size,
                            A.tile_size,
                            tileA,
                            A.tile_size,
                            tileB,
                            A.tile_size,
                            tileC,
                            A.tile_size,
                            tileS,
                            S.tile_size,
                            &work[0],
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
