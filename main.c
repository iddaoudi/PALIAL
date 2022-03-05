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
#include <string.h>
#include <getopt.h>
#include <sys/time.h>

#include "lapacke.h"
#include "cblas.h"
#include "omp.h"
#include "src/cvector.h"

int MSIZE, BSIZE, NTH;

#define PALIAL_TRACE 1
#ifdef PALIAL_TRACE
cvector_vector_type(char*) ompt_task_names = NULL;
#include "trace/trace.h"
#endif

#define A(m,n) MATRIX_tile_address(A, m, n)
#define B(m,n) MATRIX_tile_address(B, m, n)
#define S(m,n) MATRIX_tile_address(S, m, n)

//#define SPECIAL4x4 1
//#define IDENTITY 1

#include "include/descriptor.h"
#include "include/tile_address.h"
#include "include/print.h"
#include "include/populate.h"

#include "src/cholesky.h"
#include "src/qr.h"
#include "src/lu.h"

int PALIAL_allocate_tile (int M, MATRIX_desc **desc, int B)
{
    int MT = M/B;
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

int main (int argc, char* argv[])
{
    NTH = atoi(getenv("OMP_NUM_THREADS"));
    /* Command line arguments parsing */
    int arguments;
    char algorithm[16];
    struct option long_options[] = {
        {"Algorithm", required_argument, NULL, 'a'},
        {"Matrix size", required_argument, NULL, 'm'},
        {"Tile size", required_argument, NULL, 'b'},
        {NULL, no_argument, NULL, 0}
    };
    if (argc < 2)
    {
        printf("Missing arguments.\n");
        printf("Aborting...\n");
        exit(0);
    }
    while ((arguments = getopt_long(argc, argv, "a:m:b:h:", long_options, NULL)) != -1)
    {
        if (optind > 2)
        {
            switch(arguments)
            {
                case 'a':
                    if (optarg) strcpy(algorithm,optarg);
                    break;
                case 'm':
                    if (optarg) MSIZE = atoi(optarg);
                    break;
                case 'b':
                    if (optarg) BSIZE = atoi(optarg);
                    break;
                case 'h':
                    printf("HELP\n");
                    exit(0);
                case '?':
                    printf("Invalid arguments.\n");
                    printf("Aborting...\n");
                    exit(0);
            }
        }
    }

    if (MSIZE%BSIZE != 0)
    {
        printf("Tile size does not divide the matrix size. Aborting...\n");
        exit(0);
    }
    /* Structures memory allocation */
    MATRIX_desc *A = NULL;
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

    struct timeval time_start, time_finish;
    if (!strcmp(algorithm, "cholesky"))
    {
#pragma omp parallel
#pragma omp master
        gettimeofday(&time_start, NULL);
        cholesky(*A);
        gettimeofday(&time_finish, NULL);
    }
    else if (!strcmp(algorithm, "qr"))
    {
        /* Workspace allocation for QR */
        MATRIX_desc *S = NULL;
        PALIAL_allocate_tile(MSIZE, &S, BSIZE);
        
#pragma omp parallel
#pragma omp master       
        gettimeofday(&time_start, NULL);
        qr(*A, *S);
        gettimeofday(&time_finish, NULL);
        
        free(S->matrix);
        matrix_desc_destroy(&S);
    }
    else if (!strcmp(algorithm, "lu"))
    {
#pragma omp parallel
#pragma omp master
        gettimeofday(&time_start, NULL);
        lu(*A);
        gettimeofday(&time_finish, NULL);
    }

    double time = (double) (time_finish.tv_usec - time_start.tv_usec)/1000000 + (double) (time_finish.tv_sec - time_start.tv_sec);
    
//    printf("############ PALIAL library ############\n");
//    printf("Algorithm, matrix size, tile size, time\n");
//    printf("%s, %d, %d, %d, %f\n", algorithm, MSIZE, BSIZE, NTH, time);
//    printf("########################################\n");
    printf("%d, %d, %d, %f\n", MSIZE, BSIZE, NTH, time);

//    for (int i = 0; i < cvector_size(ompt_tasks); i++)
//    {
//        printf("task id   : %" PRIu64 "\n", ompt_tasks[i]->id);
//        printf("task name : %s\n", ompt_tasks[i]->name);
//        printf("task ndep : %d\n", ompt_tasks[i]->n_dependences);
//        for (int j = 0; j < ompt_tasks[i]->n_dependences; j++)
//        {
//          printf("task access mode   : %s\n", ompt_tasks[i]->access_mode[j]);
//        }
//        printf("task sched: %d\n", ompt_tasks[i]->scheduled);
//        printf("task node : %d\n", ompt_tasks[i]->node);
//        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
//    }
    
    free(A->matrix);
    matrix_desc_destroy(&A);
}
