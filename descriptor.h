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
    void* mat;
    int mb; //tile rows
    int nb; //tile columns
    int msize; //matrix number of elements
    int bsize; //tile number of elements
    int lm; //matrix rows
    int ln; //matrix columns
} MATRIX_desc;

/* Matrix descriptor initialization */
MATRIX_desc matrix_desc_init(int mb, int nb, int msize, int bsize, int lm, int ln)
{
    MATRIX_desc desc;
    desc.mat = NULL;
    desc.mb = mb;
    desc.nb = nb;
    desc.msize = msize;
    desc.bsize = bsize;
    desc.lm = lm;
    desc.ln = ln;
    return desc;
}

/* Matrix descriptor creation */
int matrix_desc_create(MATRIX_desc **desc, void *mat, int mb, int nb, int msize, int bsize, int lm, int ln)
{
    *desc = (MATRIX_desc*)malloc(sizeof(MATRIX_desc));
    **desc = matrix_desc_init(mb, nb, msize, bsize, lm, ln);
    (**desc).mat = mat;
    return 0;
}

int matrix_desc_alloc(MATRIX_desc *desc)
{
    size_t size;
    size = MSIZE * MSIZE * sizeof(double);
    desc->mat = malloc(size);
    return 0;
}

int matrix_desc_destroy(MATRIX_desc **desc)
{
    free(*desc);
    *desc = NULL;
    return 0;
}
