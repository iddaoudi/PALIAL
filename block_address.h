/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 30.01.2022
 * Last Modified Date: 30.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

void *MATRIX_block_address(MATRIX_desc A, int m, int n)
{
    size_t single_element_size = sizeof(double); //FIXME
    size_t offset = 0;

    if (m < A.mb)
    {
        if (n < A.nb)
        {
            offset = A.bsize * (m + A.mb * n);
        }
        else
        {
            offset = A.msize; //FIXME
            /* Current implementation considers that matrix size % bloc size = 0 */
        }
    }
    else
    {
        offset = A.msize; //FIXME
    }
    return (void*)((char*)A.mat + (offset*single_element_size));
}
