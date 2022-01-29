/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 28.01.2022
 * Last Modified Date: 28.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

#include <stdlib.h>
#include <stdio.h>
#include "lapacke.h"
#include "cblas.h"

#define MSIZE 4
#define BSIZE 2

struct Matrix
{
	int row_size;
	int column_size;
	double** data;
};
typedef struct Matrix Matrix;

Matrix* matrix_init(int row_size, int column_size)
{
	struct Matrix* matrix = malloc(sizeof(Matrix));
	matrix->row_size = row_size;
	matrix->column_size = column_size;
	double** data = malloc(row_size * sizeof(double*));
	for (int i = 0; i < column_size; i++)
	{
		data[i] = calloc(column_size, sizeof(double));
	}
	matrix->data = data;
	return matrix;
}

void print_matrix (Matrix* matrix_A)
{
	for (int j = 0; j < MSIZE; j++)
	{
		for (int i = 0; i < MSIZE; i++)
		{
			printf("%f ", matrix_A->data[i][j]);
			if (i == MSIZE-1)
			{
				printf("\n");
			}
		}
	}
	printf("----------------------------------\n");
}

int main ()
{
	/* Allocating matrix A the proper way */
	Matrix* matrix_A = matrix_init(MSIZE, MSIZE);
	/* Populating matrix A */	
	//for (int i = 0; i < MSIZE; i++)
	//{
	//	for (int j = 0; j < MSIZE; j++)
	//	{
	//		if (i == j)
	//			matrix_A->data[i][j] = 1.0;
	//	}
	//}
	
	matrix_A->data[0][0] = 1.0;
	matrix_A->data[0][1] = 1.0;
	matrix_A->data[0][2] = 1.0;
	matrix_A->data[0][3] = 1.0;
	matrix_A->data[1][0] = 1.0;
	matrix_A->data[1][1] = 5.0;
	matrix_A->data[1][2] = 5.0;
	matrix_A->data[1][3] = 5.0;
	matrix_A->data[2][0] = 1.0;
	matrix_A->data[2][1] = 5.0;
	matrix_A->data[2][2] = 14.0;
	matrix_A->data[2][3] = 14.0;
	matrix_A->data[3][0] = 1.0;
	matrix_A->data[3][1] = 5.0;
	matrix_A->data[3][2] = 14.0;
	matrix_A->data[3][3] = 15.0;

	print_matrix(matrix_A);

	int i, j, k;
	for (i = 0; i < BSIZE; i++)
	{
		double tileA = matrix_A->data[i][i];
		int info = LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 
				'U', 
				BSIZE, 
				&tileA, 
				BSIZE);

		printf("dpotrf done\n");	
		for (j = i+1; j < BSIZE; j++)
		{
			double tileA = matrix_A->data[i][i];
			double tileB = matrix_A->data[i][j];
			cblas_dtrsm(CblasRowMajor, 
					CblasLeft, 
					CblasUpper, 
					CblasNoTrans, 
					CblasNonUnit, 
					BSIZE, 
					BSIZE, 
					1.0, 
					&tileA, 
					BSIZE, 
					&tileB, 
					BSIZE);			
			printf("dtrsm done\n");	
		}

		for (k = i+1; k < BSIZE; k++)
		{
			double tileA = matrix_A->data[i][k];
			double tileB = matrix_A->data[k][k];
			cblas_dsyrk(CblasRowMajor, 
					CblasUpper, 
					CblasNoTrans, 
					BSIZE, 
					BSIZE, 
					-1.0, 
					&tileA, 
					BSIZE, 
					1.0, 
					&tileB, 
					BSIZE);
			printf("dsyrk done\n");	

			for (j = k+1; j < BSIZE; j++)
			{
				double tileA = matrix_A->data[i][k];
				double tileB = matrix_A->data[i][j];
				double tileC = matrix_A->data[k][j];
				cblas_dgemm(CblasRowMajor, 
						CblasTrans, 
						CblasTrans, 
						BSIZE, 
						BSIZE, 
						BSIZE, 
						-1.0, 
						&tileA, 
						BSIZE, 
						&tileB, 
						BSIZE, 
						1.0, 
						&tileC, 
						BSIZE);
				printf("dgemm done\n");	
			}
		}
	
	}

	print_matrix(matrix_A);	
	free(matrix_A->data);
}
