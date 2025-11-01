// src/vecadd_scalar.c
// Scalar baseline (no auto-vectorization) -- compile with -O3 -fno-tree-vectorize
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef ALIGN
#define ALIGN 64
#endif

static inline double drand(){
    return (double)rand() / (double)RAND_MAX;
}

int main(int argc, char** argv){
    // Usage: ./vecadd_scalar -n <N> -r <repeats> -seed <seed>
    size_t N = 1<<20;       // 1,048,576 elements default
    int repeats = 10;       // compute repeats to amplify instruction count
    unsigned int seed = 1;
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "-n")==0 && i+1<argc) N = strtoull(argv[++i], NULL, 10);
        else if(strcmp(argv[i], "-r")==0 && i+1<argc) repeats = atoi(argv[++i]);
        else if(strcmp(argv[i], "-seed")==0 && i+1<argc) seed = (unsigned int)strtoul(argv[++i], NULL, 10);
    }

    srand(seed);
    double *a=NULL, *b=NULL, *c=NULL;
    if(posix_memalign((void**)&a, ALIGN, N*sizeof(double)) ||
       posix_memalign((void**)&b, ALIGN, N*sizeof(double)) ||
       posix_memalign((void**)&c, ALIGN, N*sizeof(double))){
        fprintf(stderr, "alloc failed\n");
        return 2;
    }

    for(size_t i=0;i<N;i++){ a[i]=drand(); b[i]=drand(); c[i]=0.0; }
    volatile double sink=0.0; // keep the compiler honest
    for(int r=0;r<repeats;r++){
        for(size_t i=0;i<N;i++){
            c[i] = a[i] + b[i];
        }
        // consume one element per repeat to defeat dead-code elim
        sink += c[(r*1315423911u)%N];
    }

    // checksum to verify identical results across variants
    double checksum=0.0;
    for(size_t i=0;i<N;i++) checksum += c[i];
    printf("OK scalar N=%zu repeats=%d checksum=%.12f sink=%.12f\n", N, repeats, checksum, sink);

    free(a); free(b); free(c);
    return 0;
}
