/**
 * File              : dgetrf.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 03.02.2022
 * Last Modified Date: 03.02.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include "dgetrf_no_pivot.h"

int PALIAL_dgetrf (int tileA_size, double *tileA, int lda)
{
    int gamma;
    if (tileA_size == 0)
        return 0;
    int k = tileA_size;
    for (int i = 0; i < k; i += tileA_size)
    {
        gamma = min(tileA_size, k - i);
        int info = PALIAL_dgetrf_no_piv(&tileA[lda * i + i],
                tileA_size,
                tileA_size,
                gamma);
        assert(!info);
        if (i + gamma < tileA_size)
        {
            cblas_dtrsm(CblasColMajor,
                    CblasLeft,
                    CblasLower,
                    CblasNoTrans,
                    CblasUnit,
                    gamma,
                    tileA_size - i + gamma,
                    1.0,
                    &tileA[lda * i + i],
                    lda,
                    &tileA[lda * (i + gamma) + i],
                    lda);
            if (i + gamma < tileA_size)
            {
                cblas_dgemm(CblasColMajor,
                        CblasNoTrans,
                        CblasNoTrans,
                        tileA_size - i + gamma, 
                        tileA_size - i + gamma, 
                        gamma,
                        -1.0,
                        &tileA[lda * i + i + gamma],
                        lda,
                        &tileA[lda * (i + gamma) + i],
                        lda,
                        1.0,
                        &tileA[lda * (i + gamma) + i + gamma],
                        lda);
            }
        }
    }
    return 0;
}
