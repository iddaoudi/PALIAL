/**
 * File              : cholesky.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 31.01.2022
 * Last Modified Date: 16.04.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "assert.h"
#include "../include/common.h"

void cholesky(MATRIX_desc A)
{
   int k, m, n;
   int cpu = 0;
   for (k = 0; k < A.matrix_size/A.tile_size; k++)
   {
      double *tileA = A(k,k);
      upstream_palial_set_task_name("potrf");
#pragma omp task shared(palial_cpus) depend (inout : tileA[0:A.tile_size*A.tile_size])
      {
          LAPACKE_dpotrf(LAPACK_COL_MAJOR, 
               'U', 
               A.tile_size,
               tileA, 
               A.tile_size);
          cpu = sched_getcpu();
          upstream_palial_set_task_cpu(cpu, "potrf");
      }
      for (m = k+1; m < A.matrix_size/A.tile_size; m++)
      {
         double *tileA = A(k,k);
         double *tileB = A(k,m);
         upstream_palial_set_task_name("trsm");
#pragma omp task shared(palial_cpus) depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[0:A.tile_size*A.tile_size])
         {
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
            cpu = sched_getcpu();
            upstream_palial_set_task_cpu(cpu, "trsm");
         }
      }

      for (m = k+1; m < A.matrix_size/A.tile_size; m++)
      {
         double *tileA = A(k,m);
         double *tileB = A(m,m);
         upstream_palial_set_task_name("syrk");
#pragma omp task shared(palial_cpus) depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[0:A.tile_size*A.tile_size])
         {
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
            cpu = sched_getcpu();
            upstream_palial_set_task_cpu(cpu, "syrk");
         }

         for (n = k+1; n < m; n++)
         {
            double *tileA = A(k,n);
            double *tileB = A(k,m);
            double *tileC = A(n,m);
            upstream_palial_set_task_name("gemm");
#pragma omp task shared(palial_cpus) depend(in : tileA[0:A.tile_size*A.tile_size], tileB[0:A.tile_size*A.tile_size]) depend(inout : tileC[0:A.tile_size*A.tile_size])
            {
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
                cpu = sched_getcpu();
                upstream_palial_set_task_cpu(cpu, "gemm");
            }
         }
      }
   }
}
