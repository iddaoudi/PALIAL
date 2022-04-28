/**
 * File              : cholesky.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 31.01.2022
 * Last Modified Date: 16.04.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "assert.h"

void cholesky(MATRIX_desc A)
{
   int k, m, n;
   for (k = 0; k < A.matrix_size/A.tile_size; k++)
   {
      double *tileA = A(k,k);
      cvector_push_back(ompt_task_names, "potrf");
#pragma omp task depend (inout : tileA[0:A.tile_size*A.tile_size])
      {
          int cpu = sched_getcpu();
          printf("cpu: %d\n", cpu);
          LAPACKE_dpotrf(LAPACK_COL_MAJOR, 
               'U', 
               A.tile_size,
               tileA, 
               A.tile_size);
      }
      for (m = k+1; m < A.matrix_size/A.tile_size; m++)
      {
         double *tileA = A(k,k);
         double *tileB = A(k,m);
         cvector_push_back(ompt_task_names, "trsm");
#pragma omp task depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[0:A.tile_size*A.tile_size])
         {
            int cpu = sched_getcpu();
            printf("cpu: %d\n", cpu);
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
         }
      }

      for (m = k+1; m < A.matrix_size/A.tile_size; m++)
      {
         double *tileA = A(k,m);
         double *tileB = A(m,m);
         cvector_push_back(ompt_task_names, "syrk");
#pragma omp task depend(in : tileA[0:A.tile_size*A.tile_size]) depend(inout : tileB[0:A.tile_size*A.tile_size])
         {
            int cpu = sched_getcpu();
            printf("cpu: %d\n", cpu);
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
         }

         for (n = k+1; n < m; n++)
         {
            double *tileA = A(k,n);
            double *tileB = A(k,m);
            double *tileC = A(n,m);
            cvector_push_back(ompt_task_names, "gemm");
#pragma omp task depend(in : tileA[0:A.tile_size*A.tile_size], tileB[0:A.tile_size*A.tile_size]) depend(inout : tileC[0:A.tile_size*A.tile_size])
            {
                int cpu = sched_getcpu();
                printf("cpu: %d\n", cpu);
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
            }
         }
      }
   }
}
