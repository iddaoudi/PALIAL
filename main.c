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

#define MSIZE 16
#define BSIZE 4

#include "descriptor.h"
#include "tile_address.h"

#define A(m,n) MATRIX_tile_address(A, m, n)
#define B(m,n) MATRIX_tile_address(B, m, n)
//#define SPECIAL4x4 1

#include "print.h"
#include "populate.h"

#include "cholesky.h"

int main ()
{
    MATRIX_desc *A = NULL;
    double *ptr = 0;
    int error = posix_memalign((void**)&ptr, getpagesize(), MSIZE * MSIZE * sizeof(double)); //FIXME sizeof(double)
    if (error)
    {
        printf("Problem allocating contiguous memory.\n");
        exit(EXIT_FAILURE);
    }
    matrix_desc_create(&A, ptr, BSIZE, BSIZE, MSIZE*MSIZE, BSIZE*BSIZE, MSIZE, MSIZE);

    hermitian_positive_generator(*A);
    
    print_matrix(*A);
    
    cholesky(*A);

    printf("RESULT:\n");
    print_matrix(*A);
    
    free(A->mat);
    matrix_desc_destroy(&A);
}
