/**
 * File              : qr.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 01.02.2022
 * Last Modified Date: 03.02.2022
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
#ifdef PALIAL_TRACE
        cvector_push_back(ompt_task_names, "geqrt");
#endif
#pragma omp task depend(inout : tileA[0:S.tile_size*S.tile_size]) depend(out : tileS[0:A.tile_size*S.tile_size])
        {
            double tho[S.tile_size];
            double work[S.tile_size * S.tile_size];
            PALIAL_dgeqrt(A.tile_size,
                    S.tile_size,
                    tileA,
                    A.tile_size,
                    tileS,
                    S.tile_size,
                    &tho[0],
                    &work[0]);
        }
        for (n = k+1; n < A.matrix_size/A.tile_size; n++)
        {
            double *tileA = A(k,k);
            double *tileS = S(k,k);
            double *tileB = A(k,n);
#ifdef PALIAL_TRACE
            cvector_push_back(ompt_task_names, "ormqr");
#endif
#pragma omp task depend(in : tileA[0:S.tile_size*S.tile_size], tileS[0:A.tile_size*S.tile_size]) depend(inout : tileB[0:S.tile_size*S.tile_size])
            {
                double work[S.tile_size * S.tile_size];
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
            }
        }
        for (m = k+1; m < A.matrix_size/A.tile_size; m++)
        {
            double *tileA = A(k,k);
            double *tileS = S(m,k);
            double *tileB = A(m,k);
#ifdef PALIAL_TRACE
            cvector_push_back(ompt_task_names, "tsqrt");
#endif
#pragma omp task depend(inout : tileA[0:S.tile_size*S.tile_size], tileB[0:S.tile_size*S.tile_size]) depend(out : tileS[0:S.tile_size*A.tile_size])
            {
                double work[S.tile_size * S.tile_size];
                double tho[S.tile_size];
                PALIAL_dtsqrt(A.tile_size,
                        tileA,
                        A.tile_size,
                        tileB,
                        A.tile_size,
                        tileS,
                        S.tile_size,
                        &tho[0],
                        &work[0]);
            }
            for (n = k+1; n < A.matrix_size/A.tile_size; n++)
            {
                double *tileA = A(k,n);
                double *tileS = S(m,k);
                double *tileB = A(m,n);
                double *tileC = A(m,k);
#ifdef PALIAL_TRACE
                cvector_push_back(ompt_task_names, "tsmqr");
#endif
#pragma omp task depend(inout : tileA[0:S.tile_size*S.tile_size], tileB[0:S.tile_size*S.tile_size]) depend(in : tileC[0:S.tile_size*S.tile_size], tileS[0:A.tile_size*S.tile_size])
                {
                    double work[S.tile_size * S.tile_size];
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
                }
            }
        }
    }
}
