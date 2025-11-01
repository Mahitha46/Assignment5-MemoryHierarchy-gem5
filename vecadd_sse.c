// src/vecadd_sse.c
// SSE2 variant using intrinsics
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <emmintrin.h>

#ifndef ALIGN
#define ALIGN 64
#endif

static inline double drand(){
    return (double)rand() / (double)RAND_MAX;
}

int main(int argc, char** argv){
    size_t N = 1<<20;
    int repeats = 10;
    unsigned int seed = 1;
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "-n")==0 && i+1<argc) N = strtoull(argv[++i], NULL, 10);
        else if(strcmp(argv[i], "-r")==0 && i+1<argc) repeats = atoi(argv[++i]);
        else if(strcmp(argv[i], "-seed")==0 && i+1<argc) seed = (unsigned int)strtoul(argv[++i], NULL, 10);
    }

    // round N to multiple of 2 doubles for SSE2
    size_t N2 = (N + 1) & ~((size_t)1);

    srand(seed);
    double *a=NULL, *b=NULL, *c=NULL;
    if(posix_memalign((void**)&a, ALIGN, N2*sizeof(double)) ||
       posix_memalign((void**)&b, ALIGN, N2*sizeof(double)) ||
       posix_memalign((void**)&c, ALIGN, N2*sizeof(double))){
        fprintf(stderr, "alloc failed\n");
        return 2;
    }
    for(size_t i=0;i<N;i++){ a[i]=drand(); b[i]=drand(); }
    for(size_t i=N;i<N2;i++){ a[i]=0; b[i]=0; }
    for(size_t i=0;i<N2;i++){ c[i]=0.0; }

    volatile double sink=0.0;
    for(int r=0;r<repeats;r++){
        size_t i=0;
        for(; i+1<N2; i+=2){
            __m128d va = _mm_load_pd(&a[i]);
            __m128d vb = _mm_load_pd(&b[i]);
            __m128d vc = _mm_add_pd(va, vb);
            _mm_store_pd(&c[i], vc);
        }
        for(; i<N; i++) c[i] = a[i] + b[i]; // tail (should be none if N even)
        sink += c[(r*2654435761u)%N];
    }

    double checksum=0.0;
    for(size_t i=0;i<N;i++) checksum += c[i];
    printf("OK sse N=%zu repeats=%d checksum=%.12f sink=%.12f\n", N, repeats, checksum, sink);

    free(a); free(b); free(c);
    return 0;
}
