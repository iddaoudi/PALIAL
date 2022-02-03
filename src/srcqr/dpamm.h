/**
 * File              : dpamm.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 01.02.2022
 * Last Modified Date: 01.02.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include <assert.h>
#include <lapacke.h>
#include <cblas.h>

static inline int PALIAL_dpamm_W (int side, int transpose, int upper_lower, int M, int N, int K, int L, int v2, int v3, const double *tileA1, int lda1, double *tileA2, int lda2, const double *tileS, int lds, double *workspace, int ldw)
{
    /* Left side case */
    if (side == PALIAL_left)
    {
        if ((transpose == PALIAL_transpose && upper_lower == PALIAL_upper) ||
                (transpose == PALIAL_notranspose && upper_lower == PALIAL_lower))
        {
            /* Copy A2 into W */
            LAPACKE_dlacpy(LAPACK_COL_MAJOR,
                    'A',
                    L,
                    N,
                    &tileA2[K - L],
                    lda2,
                    workspace,
                    ldw);
            if (L > 0)
            {
                cblas_dtrmm(CblasColMajor,
                        CblasLeft, /* Left */
                        side,
                        transpose,
                        CblasNonUnit,
                        L,
                        N,
                        1.0,
                        &tileS[v2],
                        lds,
                        workspace,
                        ldw);
                if (K > L)
                {
                    cblas_dgemm(CblasColMajor,
                            transpose,
                            CblasNoTrans,
                            L,
                            N,
                            K - L,
                            1.0,
                            tileS,
                            lds,
                            tileA2,
                            lda2,
                            1.0,
                            workspace,
                            ldw);
                }
            }
            if (M > L)
            {
                cblas_dgemm(CblasColMajor,
                        transpose,
                        CblasNoTrans,
                        M - L,
                        N,
                        K,
                        1.0,
                        &tileS[v3],
                        lds,
                        tileA2,
                        lda2,
                        0.0,
                        &workspace[L],
                        ldw);
            }
            for (int j = 0; j < N; j++)
            {
                cblas_daxpy(M,
                        1.0,
                        &tileA1[lda1 * j],
                        1,
                        &workspace[ldw * j],
                        1);
            }
        }
        else
        {
            return 1;
        }
    }
    /* Right side case */
    else
    {
        if ((transpose == PALIAL_transpose && upper_lower == PALIAL_upper) ||
                (transpose == PALIAL_notranspose && upper_lower == PALIAL_lower))
            {
                return 1;
            }
        else
        {
            if (L > 0)
            {
                LAPACKE_dlacpy(LAPACK_COL_MAJOR,
                        'A',
                        M,
                        L,
                        &tileA2[lda2 * (K - L)],
                        lda2,
                        workspace,
                        ldw);
                cblas_dtrmm(CblasColMajor,
                        CblasRight,
                        upper_lower,
                        transpose,
                        CblasNonUnit,
                        M,
                        L,
                        1.0,
                        &tileS[v2],
                        lds,
                        workspace,
                        ldw);
                if (K > L)
                {
                    cblas_dgemm(CblasColMajor,
                            CblasNoTrans,
                            transpose,
                            M,
                            L,
                            K - L,
                            1.0,
                            tileA2,
                            lda2,
                            tileS,
                            lds,
                            1.0,
                            workspace,
                            ldw);
                }
            }
            if (N > L)
            {
                cblas_dgemm(CblasColMajor,
                        CblasNoTrans,
                        transpose,
                        M,
                        N - L,
                        K,
                        1.0,
                        tileA2,
                        lda2,
                        &tileS[v3],
                        lds,
                        0.0,
                        &workspace[ldw * L],
                        ldw);
            }
            for (int j = 0; j < N; j++)
            {
                cblas_daxpy(M,
                        1.0,
                        &tileA1[lda1 * j],
                        1,
                        &workspace[ldw * j],
                        1);
            }
        }
    }
    return 0;
}

static inline int PALIAL_dpamm_A2 (int side, int transpose, int upper_lower, int M, int N, int K, int L, int v2, int v3, double *tileA2, int lda2, const double *tileS, int lds, double *workspace, int ldw)
{
    if (side == PALIAL_left)
    {
        if ((transpose == PALIAL_transpose && upper_lower == PALIAL_upper) ||
                (transpose == PALIAL_notranspose && upper_lower == PALIAL_lower))
        {
            return 1;
        }
        else
        {
            if (M > L)
            {
                cblas_dgemm(CblasColMajor,
                        transpose,
                        CblasNoTrans,
                        M - L,
                        N,
                        L,
                        -1.0,
                        tileS,
                        lds,
                        workspace,
                        ldw,
                        1.0,
                        tileA2,
                        lda2);
            }
            cblas_dtrmm(CblasColMajor,
                    CblasLeft,
                    upper_lower,
                    transpose,
                    CblasNonUnit,
                    L,
                    N,
                    1.0,
                    &tileS[v2],
                    lds,
                    workspace,
                    ldw);
            for (int j = 0; j < N; j++)
            {
                cblas_daxpy(L,
                        -1.0,
                        &workspace[ldw * j],
                        1,
                        &tileA2[lda2 * j + (M - L)],
                        1);
            }
            if (K > L)
            {
                cblas_dgemm(CblasColMajor,
                        transpose,
                        CblasNoTrans,
                        M,
                        N,
                        K - L,
                        -1.0,
                        &tileS[v3],
                        lds,
                        &workspace[L],
                        ldw,
                        1.0,
                        tileA2,
                        lda2);
            }
        }
    }
    else
    {
        if ((transpose == PALIAL_transpose && upper_lower == PALIAL_upper) ||
                (transpose == PALIAL_notranspose && upper_lower == PALIAL_lower))
        {
            if (K > L)
            {
                cblas_dgemm(CblasColMajor,
                        CblasNoTrans,
                        transpose,
                        M,
                        N,
                        K - L,
                        -1.0,
                        &workspace[ldw * L],
                        ldw,
                        &tileS[v3],
                        lds,
                        1.0,
                        tileA2,
                        lda2);
            }
            if (N > L)
            {
                cblas_dgemm(CblasColMajor,
                        CblasNoTrans,
                        transpose,
                        M,
                        N - L,
                        L,
                        -1.0,
                        workspace,
                        ldw,
                        tileS,
                        lds,
                        1.0,
                        tileA2,
                        lda2);
            }
            if (L > 0)
            {
                cblas_dtrmm(CblasColMajor,
                        CblasRight,
                        upper_lower,
                        transpose,
                        CblasNonUnit,
                        M,
                        L,
                        -1.0,
                        &tileS[v2],
                        lds,
                        workspace,
                        ldw);
                for (int j = 0; j < L; j++)
                {
                    cblas_daxpy(M,
                            1.0,
                            &workspace[ldw * j],
                            1,
                            &tileA2[lda2 * (N - L + j)],
                            1);
                }
            }
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

static inline int PALIAL_dpamm (int operation, int side, int store_column_row, int M, int N, int K, int L, const double *tileA1, int lda1, double *tileA2, int lda2, const double *tileS, int lds, double *workspace, int ldw)
{
    int upper_lower, transpose, v2, v3;
    /* If sizes are equal to zero, nothing to do here */
    if (M == 0 || N == 0 || K == 0)
        return 0;
    /* Column wise case */
    if (store_column_row == PALIAL_column)
    {
        upper_lower = PALIAL_upper;
        if (side == PALIAL_left)
        {
            transpose = operation == PALIAL_A2 ? PALIAL_notranspose : PALIAL_transpose;
            v2 = transpose == PALIAL_notranspose ? M - L : K - L;
        }
        else
        {
            transpose = operation == PALIAL_W ? PALIAL_notranspose : PALIAL_transpose;
            v2 = transpose == PALIAL_notranspose ? K - L : N - L;
        }
        v3 = lds * L;
    }
    /* Row wise case */
    else
    {
        upper_lower = PALIAL_lower;
        if (side == PALIAL_left)
        {
            transpose = operation == PALIAL_W ? PALIAL_notranspose : PALIAL_transpose;
            v2 = transpose == PALIAL_notranspose ? K - L : M - L;
        }
        else
        {
            transpose = operation == PALIAL_A2 ? PALIAL_notranspose : PALIAL_transpose;
            v2 = transpose == PALIAL_notranspose ? N - L : K - L;
        }
        v2 = v2 * lds;
        v3 = L;
    }
    /* According to the operation (either on A2 or W), start the corresponding kernel */
    if (operation == PALIAL_W)
    {
        int info = PALIAL_dpamm_W(side,
                transpose,
                upper_lower,
                M,
                N,
                K,
                L,
                v2,
                v3,
                tileA1,
                lda1,
                tileA2,
                lda2,
                tileS,
                lds,
                workspace,
                ldw);
        assert(!info);
    }
    else if (operation == PALIAL_A2)
    {
        int info = PALIAL_dpamm_A2(side,
                transpose,
                upper_lower,
                M,
                N,
                K,
                L,
                v2,
                v3,
                tileA2,
                lda2,
                tileS,
                lds,
                workspace,
                ldw);
        assert(!info);
    }
    return 0;
}
