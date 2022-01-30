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

#define MSIZE 6
#define BSIZE 2

#include "descriptor.h"
#include "block_address.h"
#define A(m,n) MATRIX_block_address(A, m, n)

#include "print.h"

//struct Matrix
//{
//    /* Square matrix required */
//    int size;
//    double** data;
//};
//typedef struct Matrix Matrix;
//
//Matrix* matrix_init(int size)
//{
//    struct Matrix *matrix = (Matrix*)malloc(sizeof(Matrix));
//    matrix->size = size;
//    double **data = malloc(size * sizeof(double*));
//    for (int i = 0; i < size; i++)
//    {
//        data[i] = malloc(size * sizeof(double));
//    }
//    matrix->data = data;
//    return matrix;
//}

void cholesky(MATRIX_desc A)
{
    int k, m, n;
    for (k = 0; k < BSIZE; k++)
    {
        //double *tileA = &matrix_A->data[k][k];
        double *tileA = A(k,k);
        //printf("%f\n", (double*) A(k,k));
        int info = LAPACKE_dpotrf(LAPACK_COL_MAJOR, 
                'U', 
                BSIZE, 
                tileA, 
                BSIZE);
        printf("dpotrf: %d\n", k);	
        
        for (m = k+1; m < BSIZE; m++)
        {
            //double *tileA = &matrix_A->data[k][k];
            //double *tileB = &matrix_A->data[k][m]; //FIXME
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
            //double *tileA = &matrix_A->data[k][m]; //FIXME
            //double *tileB = &matrix_A->data[m][m];
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
                //double* tileA = &matrix_A->data[k][n]; //FIXME
                //double* tileB = &matrix_A->data[k][m];
                //double* tileC = &matrix_A->data[n][m];
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
    for (int i = 0; i < A.lm; i++)
    {
        for (int j = 0; j < A.ln; j++)
        {
            double *dA = A(i, j);
            *dA = 0.0;
        }
    }
}

int main ()
{
    /* Allocating matrix A the proper way */
    //Matrix* matrix_A = matrix_init(MSIZE);
    /* Populating matrix A */	
    //for (int i = 0; i < MSIZE; i++)
    //{
    //	for (int j = 0; j < MSIZE; j++)
    //	{
    //		if (i == j)
    //			matrix_A->data[i][j] = 0.0;
    //	}
    //}

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

    //print_matrix(matrix_A);
    
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

    cholesky(*A);

    free(A->mat);
    matrix_desc_destroy(&A);
    //print_matrix(matrix_A, MSIZE);	
    //free(matrix_A->data);
    //free(matrix_A);
}
