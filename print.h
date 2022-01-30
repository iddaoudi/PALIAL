/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 30.01.2022
 * Last Modified Date: 30.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

void print_matrix (MATRIX_desc A)
{
    printf("Printing matrix:\n");
    double *B = malloc(A.lm * A.ln * sizeof(double));
    int tile_counter = 0;
    int matrix_counter = 0;
    for (int i = 0; i < A.lm/A.mb; i++)
    {
        for (int j = 0; j < A.ln/A.nb; j++)
        {
            double *dA = A(j,i);
            for (int k = 0; k < A.mb; k++)
            {
                for (int l = 0; l < A.nb; l++)
                {
                    B[k*A.mb + l + tile_counter + matrix_counter + k*A.mb] = dA[k*A.mb + l];
                }
            }
            tile_counter += A.mb;
        }
        matrix_counter += A.lm;
    }
    for (int i = 0; i < A.lm*A.ln; i++)
    {
        printf("%f ", B[i]);
        if ((i+1)%A.lm == 0)
            printf("\n");
    }
}
