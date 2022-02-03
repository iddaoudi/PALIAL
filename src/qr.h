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
    int k, m, n;
    for (k = 0; k < A.matrix_size/A.tile_size; k++)
    {
        double *tileA = A(k,k);
        double *tileS = A(k,k);
        double tho[S.tile_size];
        double work[S.tile_size * S.tile_size];
        
        int info = PALIAL_dgeqrt(A.tile_size,
                S.tile_size,
                tileA,
                A.tile_size,
                tileS,
                S.tile_size,
                &tho[0],
                &work[0]);
        assert(!info);
        for (n = k+1; n < A.matrix_size/A.tile_size; n++)
        {
            double *tileA = A(k,k);
            double *tileS = S(k,k);
            double *tileB = A(k,n);
            double work[S.tile_size * S.tile_size];
            
            int info = PALIAL_dormqr(PALIAL_left,
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
            assert(!info);
        }
        for (m = k+1; m < A.matrix_size/A.tile_size; m++)
        {
            double *tileA = A(k,k);
            double *tileS = S(m,k);
            double *tileB = A(m,k);
            double tho[S.tile_size];
            double work[S.tile_size * S.tile_size];
            
            int info = PALIAL_dtsqrt(A.tile_size,
                    tileA,
                    A.tile_size,
                    tileB,
                    A.tile_size,
                    tileS,
                    S.tile_size,
                    &tho[0],
                    &work[0]);
            assert(!info);
            for (n = k+1; n < A.matrix_size/A.tile_size; n++)
            {
                double *tileA = A(k,n);
                double *tileS = S(m,k);
                double *tileB = A(m,n);
                double *tileC = A(m,k);
                double work[S.tile_size * S.tile_size];
                
                int info = PALIAL_dtsmqr(PALIAL_left,
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
                assert(!info);
            }
        }
    }

}
