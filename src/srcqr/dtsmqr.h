/**
 * File              : dtsmqr.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 01.02.2022
 * Last Modified Date: 03.02.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "dparfb.h"

int PALIAL_dtsmqr (int side, int transpose, int M, int N, int K, int B, double *tileA1, int lda1, double *tileA2, int lda2, const double *tileS, int lds, const double *tileB, int ldb, double *workspace, int ldw)
{
    if (M == 0 || N == 0 || K == 0 || B == 0)
        return 0;

    int i1, i3, gamma;
    int ic = 0;
    int m = M;
    if ((side == PALIAL_left && transpose != PALIAL_notranspose) ||
            (side == PALIAL_right && transpose == PALIAL_notranspose))
    {
        i1 = 0;
        i3 = B;
    }
    else
    {
        i1 = K - 1;
        i3 = -B;
    }

    for (int i = i1; i > -1 && i < K; i += i3)
    {
        gamma = min(B, K - i);
        if (side == PALIAL_left)
        {
            m = M - i;
            ic = i;
        }
        PALIAL_dparfb(side,
                transpose,
                PALIAL_forward, 
                PALIAL_column,
                m,
                N,
                gamma,
                0,
                &tileA1[lda1 * ic + ic],
                lda1,
                tileA2,
                lda2,
                &tileS[lds * i],
                lds,
                &tileB[ldb * i],
                ldb,
                workspace,
                ldw);
    }
    return 0;
}
