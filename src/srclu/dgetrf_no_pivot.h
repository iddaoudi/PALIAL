/**
 * File              : dgetrf_no_pivot.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 03.02.2022
 * Last Modified Date: 03.02.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

#include <math.h>

int PALIAL_dgetrf_no_piv (double *tileA, int lda, int tileA_size, int internal_size)
{
    if (tileA_size == 0 || internal_size == 0)
        return 0;
    double alpha, safe_minimum;
    safe_minimum = LAPACKE_dlamch_work('S');
    int k = min(tileA_size, internal_size);
    for (int i = 0; i < k; i++)
    {
        alpha = tileA[i * lda + i];
        if (alpha != 0.0)
        {
            if (i < tileA_size)
            {
                if (abs(alpha) > safe_minimum)
                {
                    alpha = 1.0/alpha;
                    cblas_dscal(tileA_size - i - 1,
                            alpha,
                            &tileA[i * lda + i + 1],
                            1);
                }
                else
                {
                    for (int j = i+1; j < tileA_size; j++)
                    {
                        tileA[lda * i + j] = tileA[lda * i + j] / alpha;
                    }
                }
            }
        }
        if (i < k)
        {
            cblas_dger(CblasColMajor,
                    tileA_size - i - 1,
                    internal_size - i - 1,
                    -1.0,
                    &tileA[lda * i + i + 1],
                    1,
                    &tileA[lda * (i + 1) + i],
                    lda,
                    &tileA[lda * (i + 1) + i + 1],
                    lda);
        }
    }
    return 0;
}
