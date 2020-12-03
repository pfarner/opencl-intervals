__kernel void vector_add(__global double *A, __global double *B, __global double *C) {
    
    // Get the index of the current element
    int i = get_global_id(0);

    // Do the operation
    C[i] = nextafter(A[i] + B[i], 10240);
}
