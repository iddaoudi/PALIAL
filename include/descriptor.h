/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 30.01.2022
 * Last Modified Date: 30.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

#include <stdlib.h>
#include <stdio.h>

/* Definition of descriptor structure */
typedef struct matrix_descriptor_t
{
    int      tile_size;        //mb
    long int matrix_nelements;
    long int tile_nelements;
    int      matrix_size;
    void*    matrix;
} MATRIX_desc;

/* Matrix descriptor initialization */
MATRIX_desc matrix_desc_init(int tile_size, long int matrix_nelements, long int tile_nelements, int matrix_size)
{
    MATRIX_desc desc;
    desc.matrix = NULL;
    desc.tile_size        = tile_size;
    desc.matrix_nelements = matrix_nelements;
    desc.tile_nelements   = tile_nelements;
    desc.matrix_size      = matrix_size;
    return desc;
}

/* Matrix descriptor creation */
int matrix_desc_create(MATRIX_desc **desc, void *matrix, int tile_size, long int matrix_nelements, long int tile_nelements, int matrix_size)
{
    *desc = (MATRIX_desc*)malloc(sizeof(MATRIX_desc));
    **desc = matrix_desc_init(tile_size, matrix_nelements, tile_nelements, matrix_size);
    (**desc).matrix = matrix;
    return 0;
}

int matrix_desc_alloc(MATRIX_desc *desc)
{
    size_t size;
    size = desc->matrix_size * desc->matrix_size * sizeof(double);
    desc->matrix = malloc(size);
    return 0;
}

int matrix_desc_destroy(MATRIX_desc **desc)
{
    free(*desc);
    *desc = NULL;
    return 0;
}
