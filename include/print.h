/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 30.01.2022
 * Last Modified Date: 31.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

#include <stdio.h>

void print_matrix (MATRIX_desc A)
{
    double *B = malloc(A.matrix_size * A.matrix_size * sizeof(double));
    int matrix_counter = 0;
    int tile_counter   = 0;
    
    printf("Printing matrix:\n");
    for (int i = 0; i < A.matrix_size/A.tile_size; i++)
    {
        for (int j = 0; j < A.matrix_size/A.tile_size; j++)
        {
            double *dA = A(i,j);
            for (int k = 0; k < A.tile_size; k++)
            {
                for (int l = 0; l < A.tile_size; l++)
                {
                    B[k*A.matrix_size + l + A.tile_size*j*A.matrix_size + i*A.tile_size] = dA[k*A.tile_size + l];
                }
            }
            tile_counter += A.tile_size;
        }
        matrix_counter += A.matrix_size;
    }
    for (int i = 0; i < A.matrix_size*A.matrix_size; i++)
    {
        printf("%f ", B[i]);
        if ((i+1)%A.matrix_size == 0)
            printf("\n");
    }
}

void simple_print_matrix (MATRIX_desc A)
{
    int counter = 0;
    for (int i = 0; i < A.matrix_size/A.tile_size; i++)
    {
        for (int j = 0; j < A.matrix_size/A.tile_size; j++)
        {
            printf("i, j: %d, %d\n", i, j);
            double *dA = A(i,j);
            for (int k = 0; k < A.tile_size; k++)
            {
                for (int l = 0; l < A.tile_size; l++)
                {
                    printf("%f ", dA[k*A.tile_size + l]);
                }
                printf("\n");
            }
            printf("\n");
            counter++;
        }
    }
}
