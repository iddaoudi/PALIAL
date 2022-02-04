/**
 * File              : populate.h
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 30.01.2022
 * Last Modified Date: 31.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

#include <time.h>

void hermitian_positive_generator (MATRIX_desc A)
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
#else
    srand((unsigned int)time(NULL));
    for (int i = 0; i < A.matrix_size/A.tile_size; i++)
    {
        for (int j = 0; j < A.matrix_size/A.tile_size; j++)
        {
            double *dA = A(i, j);
            for (int k = 0; k < A.tile_size; k++)
            {
                for (int l = 0; l < A.tile_size; l++)
                {
                    /* Random diagonal elements on the diagonal tiles of matrix */
                    if (i == j && k == l) 
                    {
#ifdef IDENTITY
                        dA[k*A.tile_size + l] = 1.0;
#else
                        double seed = 173.0;
                        dA[k*A.tile_size + l] = ((double)rand()/(double)(RAND_MAX)) * seed + seed;
#endif
                    }
                    /* Fixed small value for all the rest */
                    else
                    {
#ifdef IDENTITY
                        dA[k*A.tile_size + l] = 0.0;
#else
                        dA[k*A.tile_size + l] = 0.5;
#endif
                    }
                }
            }
        }
    }
#endif
}
