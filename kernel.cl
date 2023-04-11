kernel void vector_add(global const int *A,global const int *B, global int *C, constant int* D) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    // Do the operations
    C[i] = A[i] + B[i] + D[0];
}