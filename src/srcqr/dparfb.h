/**
 * File              : dparfb.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 01.02.2022
 * Last Modified Date: 02.02.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include <lapacke.h>
#include <cblas.h>

#include "dpamm.h"

int PALIAL_dparfb (int side, int transpose, int direct, int store_column_row, int M, int N, int K, int L, double *tileA1, int lda1, double *tileA2, int lda2, const double *tileS, int lds, const double *tileB, int ldb, double *workspace, int ldw)
{
    if (M == 0 || N == 0 || K == 0)
        return 0;

    if (direct == PALIAL_forward)
    {
        if (side == PALIAL_left)
        {
            PALIAL_dpamm(PALIAL_W,
                    PALIAL_left,
                    store_column_row,
                    K,
                    M,
                    N,
                    L,
                    tileA1,
                    lda1,
                    tileA2,
                    lda2,
                    tileS,
                    lds,
                    workspace,
                    ldw);
            cblas_dtrmm(CblasColMajor,
                    CblasLeft,
                    CblasUpper,
                    transpose,
                    CblasNonUnit,
                    K,
                    N,
                    1.0,
                    tileB,
                    ldb,
                    workspace,
                    ldw);
            for (int j = 0; j < M; j++)
            {
                cblas_daxpy(K,
                        -1.0, 
                        &workspace[ldw * j],
                        1,
                        &tileA1[lda1 * j],
                        1);
            }
            PALIAL_dpamm(PALIAL_A2,
                    PALIAL_left,
                    store_column_row,
                    N,
                    N,
                    K,
                    L,
                    tileA1,
                    lda1,
                    tileA2,
                    lda2,
                    tileS,
                    lds,
                    workspace,
                    ldw);
        }
        else
        {
            PALIAL_dpamm(PALIAL_W,
                    PALIAL_right,
                    store_column_row,
                    M,
                    K,
                    N,
                    L,
                    tileA1,
                    lda1,
                    tileA2,
                    lda2,
                    tileS,
                    lds,
                    workspace,
                    ldw);
            cblas_dtrmm(CblasColMajor,
                    CblasRight,
                    CblasUpper,
                    transpose,
                    CblasNonUnit,
                    N,
                    K, 
                    1.0,
                    tileB,
                    ldb,
                    workspace,
                    ldw);
            for (int j = 0; j < K; j++)
            {
                cblas_daxpy(M,
                        -1.0,
                        &workspace[ldw * j],
                        1,
                        &tileA1[lda1 * j],
                        1);
            }
            PALIAL_dpamm(PALIAL_A2,
                    PALIAL_right,
                    store_column_row,
                    N,
                    N,
                    K,
                    L,
                    tileA1,
                    lda1,
                    tileA2,
                    lda2,
                    tileS,
                    lds,
                    workspace,
                    ldw);
        }
    }
    else
    {
        return 1;
    }
    return 0;
}
