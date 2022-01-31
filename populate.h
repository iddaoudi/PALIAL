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
    for (int i = 0; i < A.lm/A.mb; i++)
    {
        for (int j = 0; j < A.ln/A.nb; j++)
        {
            double *dA = A(i, j);
            for (int k = 0; k < A.mb; k++)
            {
                for (int l = 0; l < A.nb; l++)
                {
                    /* Random diagonal elements on the diagonal tiles of matrix */
                    if (i == j && k == l) 
                    {
                        double seed = 173.0;
                        dA[k*A.mb + l] = ((double)rand()/(double)(RAND_MAX)) * seed;
                    }
                    /* Fixed small value for all the rest */
                    else
                    {
                        dA[k*A.mb + l] = 0.5;
                    }
                }
            }
        }
    }
#endif
}
