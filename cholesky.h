/**
 * File              : cholesky.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 31.01.2022
 * Last Modified Date: 31.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 */

void cholesky(MATRIX_desc A)
{
    int k, m, n;
    for (k = 0; k < MSIZE/BSIZE; k++) //FIXME
    {
        double *tileA = A(k,k);
        int info = LAPACKE_dpotrf(LAPACK_COL_MAJOR, 
                'U', 
                BSIZE, 
                tileA, 
                BSIZE);
        
        for (m = k+1; m < MSIZE/BSIZE; m++)
        {
            double *tileA = A(k,k);
            double *tileB = A(k,m);
            cblas_dtrsm(CblasColMajor, 
                    CblasLeft, 
                    CblasUpper, 
                    CblasTrans, 
                    CblasNonUnit, 
                    BSIZE, 
                    BSIZE, 
                    1.0, 
                    tileA, 
                    BSIZE, 
                    tileB, 
                    BSIZE);			
        }

        for (m = k+1; m < MSIZE/BSIZE; m++)
        {
            double *tileA = A(k,m);
            double *tileB = A(m,m);
            cblas_dsyrk(CblasColMajor, 
                    CblasUpper, 
                    CblasTrans, 
                    BSIZE, 
                    BSIZE, 
                    -1.0, 
                    tileA, 
                    BSIZE, 
                    1.0, 
                    tileB, 
                    BSIZE);

            for (n = k+1; n < m; n++)
            {
                double *tileA = A(k,n);
                double *tileB = A(k,m);
                double *tileC = A(n,m);
                cblas_dgemm(CblasColMajor, 
                        CblasTrans, 
                        CblasNoTrans, 
                        BSIZE, 
                        BSIZE, 
                        BSIZE, 
                        -1.0, 
                        tileA, 
                        BSIZE, 
                        tileB, 
                        BSIZE, 
                        1.0, 
                        tileC, 
                        BSIZE);
            }
        }
    }
}
