/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 28.01.2022
 * Last Modified Date: 30.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lapacke.h"
#include "cblas.h"

#define MSIZE 4
#define BSIZE 2

#include "descriptor.h"
#include "tile_address.h"
#define A(m,n) MATRIX_tile_address(A, m, n)

#include "print.h"

#define SPECIAL4x4 1

void cholesky(MATRIX_desc A)
{
    int k, m, n;
    for (k = 0; k < BSIZE; k++)
    {
        double *tileA = A(k,k);
        int info = LAPACKE_dpotrf(LAPACK_COL_MAJOR, 
                'U', 
                BSIZE, 
                tileA, 
                BSIZE);
        printf("dpotrf: %d\n", k);	
        
        for (m = k+1; m < BSIZE; m++)
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
            printf("dtrsm: %d\n", m);
        }

        for (m = k+1; m < BSIZE; m++)
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
            printf("dsyrk: %d\n", m);	

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
                printf("dgemm: %d\n", n);	
            }
        }

    }
}

void matrix_populate(MATRIX_desc A)
{
    
#ifdef SPECIAL4x4
    double *dA = A(0, 0);
    dA[0] = 4.5;
    dA[1] = -0.095026;
    dA[2] = -0.095026;
    dA[3] = 3.719688;
    dA = A(1, 0);
    dA[0] = 0.361857;
    dA[1] = 0.388551;
    dA[2] = -0.447549;
    dA[3] = 0.155058;
    dA = A(0, 1);
    dA[0] = 0.361857;
    dA[1] = -0.447549;
    dA[2] = 0.388551;
    dA[3] = 0.155058;
    dA = A(1, 1);
    dA[0] = 4.484953;
    dA[1] = 0.342457;
    dA[2] = 0.342457;
    dA[3] = 3.982519;
#endif

}

int main ()
{
    //matrix_A->data[0][0] = 1.0;
    //matrix_A->data[0][1] = 1.0;
    //matrix_A->data[0][2] = 1.0;
    //matrix_A->data[0][3] = 1.0;
    //matrix_A->data[1][0] = 1.0;
    //matrix_A->data[1][1] = 5.0;
    //matrix_A->data[1][2] = 5.0;
    //matrix_A->data[1][3] = 5.0;
    //matrix_A->data[2][0] = 1.0;
    //matrix_A->data[2][1] = 5.0;
    //matrix_A->data[2][2] = 14.0;
    //matrix_A->data[2][3] = 14.0;
    //matrix_A->data[3][0] = 1.0;
    //matrix_A->data[3][1] = 5.0;
    //matrix_A->data[3][2] = 14.0;
    //matrix_A->data[3][3] = 15.0;

    MATRIX_desc *A = NULL;
    double *ptr = 0;
    int error = posix_memalign((void**)&ptr, getpagesize(), MSIZE * MSIZE* sizeof(double)); //FIXME sizeof(double)
    if (error)
    {
        printf("Problem allocating contiguous memory.\n");
        exit(EXIT_FAILURE);
    }
    matrix_desc_create(&A, ptr, BSIZE, BSIZE, MSIZE*MSIZE, BSIZE*BSIZE, MSIZE, MSIZE);

    matrix_populate(*A); 
    print_matrix(*A);
    
    cholesky(*A);

    print_matrix(*A);
    
    free(A->mat);
    matrix_desc_destroy(&A);
}
