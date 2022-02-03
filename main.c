/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 28.01.2022
 * Last Modified Date: 31.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lapacke.h"
#include "cblas.h"

#define MSIZE 8192
#define BSIZE 512

#define A(m,n) MATRIX_tile_address(A, m, n)
#define B(m,n) MATRIX_tile_address(B, m, n)
#define S(m,n) MATRIX_tile_address(S, m, n)
//#define SPECIAL4x4 1
#define IDENTITY 1

#include "include/descriptor.h"
#include "include/tile_address.h"

#include "include/print.h"
#include "include/populate.h"

#include "src/cholesky.h"
#include "src/qr.h"
#include "src/lu.h"

int PALIAL_allocate_tile (int M, MATRIX_desc **desc, int B)
{
    int MT = M/B; //FIXME no need for M and N (squared matrix)
    *desc = (MATRIX_desc*)malloc(sizeof(MATRIX_desc));
    if (*desc == NULL)
    {
        printf("Tile allocation failed.\n");
        exit(1);
    }
    **desc = matrix_desc_init(B, MT * B, B * B, MT * B);
    int info = matrix_desc_alloc(*desc);
    assert(!info);
    return 0;
}

int main ()
{
    /* Structures memory allocation */
    MATRIX_desc *A = NULL;
    MATRIX_desc *S = NULL;
    double *ptr = 0;
    int error = posix_memalign((void**)&ptr, getpagesize(), MSIZE * MSIZE * sizeof(double));
    if (error)
    {
        printf("Problem allocating contiguous memory.\n");
        exit(EXIT_FAILURE);
    }
    /* Matrix creation */
    matrix_desc_create(&A, ptr, BSIZE, MSIZE*MSIZE, BSIZE*BSIZE, MSIZE);
    /* Creating Hermitian positive matrix */
    hermitian_positive_generator(*A);
    /* Workspace allocation for QR */
    PALIAL_allocate_tile(MSIZE, &S, BSIZE);

    //print_matrix(*A);
    
    //cholesky(*A);
    //qr(*A, *S);
    lu(*A);

    free(A->matrix);
    free(S->matrix);
    matrix_desc_destroy(&A);
    matrix_desc_destroy(&S);
}
