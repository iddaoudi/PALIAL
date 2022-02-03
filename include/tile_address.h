/**
 * File              : cholesky.c
 * Author            : Idriss Daoudi <idaoudi@anl.gov>
 * Date              : 30.01.2022
 * Last Modified Date: 31.01.2022
 * Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>
 **/

static inline void *MATRIX_tile_address(MATRIX_desc A, int m, int n)
{
    size_t single_element_size = sizeof(double); //FIXME
    size_t offset = 0;

    /* Current implementation considers that matrix size % bloc size = 0 */
    offset = A.tile_nelements * (m + A.matrix_size/A.tile_size * n);
    return (void*)((char*)A.matrix + (offset*single_element_size));
}
