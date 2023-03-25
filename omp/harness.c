#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "gtmp.h"
#include <sys/time.h>

int main(int argc, char** argv)
{
    int num_threads, num_iter = 10;

    double total_time = 0.0;

    if (argc < 2) {
      fprintf(stderr, "Usage: ./harness [NUM_THREADS]\n");
      exit(EXIT_FAILURE);
    }
    num_threads = strtol(argv[1], NULL, 10);

    omp_set_dynamic(0);
    if (omp_get_dynamic())
      printf("Warning: dynamic adjustment of threads has been set\n");

    omp_set_num_threads(num_threads);
    
    gtmp_init(num_threads);

#pragma omp parallel shared(num_threads)
   {
      struct timeval t1, t2;
      gettimeofday(&t1, NULL);

      int i;
      for(i = 0; i < num_iter; i++) {
        gtmp_barrier();
      }

      gettimeofday(&t2, NULL);

      total_time += (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
   }

    gtmp_finalize();

    printf("Number of Threads: %d\tAverage time %f\n", num_threads, total_time / num_threads);

    return 0;
}
