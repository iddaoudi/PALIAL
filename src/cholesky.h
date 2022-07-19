/**
 * File              : cholesky.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 31.01.2022
 * Last Modified Date: 14.07.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "assert.h"
#include "../include/common.h"

void cholesky(MATRIX_desc A)
{
   int k = 0, m = 0, n = 0;
   for (k = 0; k < A.matrix_size/A.tile_size; k++)
   {
      double *tileA = A(k,k);
      
      // char array holding the task name and a unique identifier
      char name_with_id_char[32];
      strncpy(name_with_id_char, "potrf", 32);
      char k_to_str[8], m_to_str[8], n_to_str[8];
      sprintf(k_to_str, "%d", k);
      sprintf(m_to_str, "%d", m);
      sprintf(n_to_str, "%d", n);
      strcat(name_with_id_char, k_to_str);
      strcat(name_with_id_char, m_to_str);
      strcat(name_with_id_char, n_to_str);
      
      upstream_palial_set_task_name(name_with_id_char);

#pragma omp task firstprivate(name_with_id_char) depend (inout : tileA[0:A.tile_size*A.tile_size])
      {
          struct timeval start, end;
          unsigned int cpu, node;
          getcpu(&cpu, &node);
          upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
          
          gettimeofday(&start, NULL);
          LAPACKE_dpotrf(LAPACK_COL_MAJOR, 
               'U', 
               A.tile_size,
               tileA, 
               A.tile_size);
          gettimeofday(&end, NULL);
          
          upstream_palial_get_task_time(start, end, name_with_id_char);
      }
      for (m = k+1; m < A.matrix_size/A.tile_size; m++)
      {
         double *tileA = A(k,k);
         double *tileB = A(k,m);
         
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
#pragma omp task firstprivate(name_with_id_char) depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[0:A.tile_size*A.tile_size])
         {
            struct timeval start, end;
            unsigned int cpu, node;
            getcpu(&cpu, &node);
            upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
            
            gettimeofday(&start, NULL);
            cblas_dtrsm(CblasColMajor, 
                  CblasLeft, 
                  CblasUpper, 
                  CblasTrans, 
                  CblasNonUnit, 
                  A.tile_size, 
                  A.tile_size, 
                  1.0, 
                  tileA, 
                  A.tile_size, 
                  tileB, 
                  A.tile_size);
            gettimeofday(&end, NULL);
            
            upstream_palial_get_task_time(start, end, name_with_id_char);
         }
      }

      for (m = k+1; m < A.matrix_size/A.tile_size; m++)
      {
         double *tileA = A(k,m);
         double *tileB = A(m,m);
         
         char name_with_id_char[32];
         strncpy(name_with_id_char, "syrk", 32);
         char k_to_str[8], m_to_str[8], n_to_str[8];
         sprintf(k_to_str, "%d", k);
         sprintf(m_to_str, "%d", m);
         sprintf(n_to_str, "%d", n);
         strcat(name_with_id_char, k_to_str);
         strcat(name_with_id_char, m_to_str);
         strcat(name_with_id_char, n_to_str);
         
         upstream_palial_set_task_name(name_with_id_char);
#pragma omp task firstprivate(name_with_id_char) depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[0:A.tile_size*A.tile_size])
         {
            struct timeval start, end;
            unsigned int cpu, node;
            getcpu(&cpu, &node);
            upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
            
            gettimeofday(&start, NULL);
            cblas_dsyrk(CblasColMajor, 
                  CblasUpper, 
                  CblasTrans, 
                  A.tile_size, 
                  A.tile_size, 
                  -1.0, 
                  tileA, 
                  A.tile_size, 
                  1.0, 
                  tileB, 
                  A.tile_size);
            gettimeofday(&end, NULL);
            
            upstream_palial_get_task_time(start, end, name_with_id_char);
         }

         for (n = k+1; n < m; n++)
         {
            double *tileA = A(k,n);
            double *tileB = A(k,m);
            double *tileC = A(n,m);
            
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
#pragma omp task firstprivate(name_with_id_char) depend(in : tileA[0:A.tile_size*A.tile_size], tileB[0:A.tile_size*A.tile_size]) depend(inout : tileC[0:A.tile_size*A.tile_size])
            {   
                struct timeval start, end;
                unsigned int cpu, node;
                getcpu(&cpu, &node);
                upstream_palial_set_task_cpu_node(cpu, node, name_with_id_char);
                
                gettimeofday(&start, NULL);
                cblas_dgemm(CblasColMajor, 
                     CblasTrans, 
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
                
                upstream_palial_get_task_time(start, end, name_with_id_char);
            }
         }
      }
   }
}
