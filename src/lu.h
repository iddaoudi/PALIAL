/**
 * File              : lu.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 03.02.2022
 * Last Modified Date: 03.02.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "srclu/dgetrf.h"

void lu (MATRIX_desc A)
{
    for (int k = 0; k < A.matrix_size/A.tile_size; k++)
    {
        double *tileA = A(k,k);
        int info = PALIAL_dgetrf(A.tile_size,
                tileA,
                A.tile_size);
        assert(!info);
        for (int m = k+1; m < A.matrix_size/A.tile_size; m++)
        {
            double *tileA = A(k,k);
            double *tileB = A(m,k);
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
        }
        for (int n = k + 1; n < A.matrix_size/A.tile_size; n++)
        {
            double *tileA = A(k,k);
            double *tileB = A(k,n);
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
            for (int m = k + 1; m < A.matrix_size/A.tile_size; m++)
            {
                double *tileA = A(m,k);
                double *tileB = A(k,n);
                double *tileC = A(m,n);
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
            }
        }
    }
}
