#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include "combined.h"
#include <sys/time.h>

int main(int argc, char** argv)
{
    int num_processes, num_threads, num_iter = 10;

    double total_time = 0.0;

    MPI_Init(&argc, &argv);

    if (argc < 3) {
        fprintf(stderr, "Usage: ./harness [NUM_PROCS] [NUM_THREADS]\n");
        exit(EXIT_FAILURE);
    }

    num_processes = strtol(argv[1], NULL, 10);
    num_threads = strtol(argv[2], NULL, 10);

    omp_set_dynamic(0);
    if (omp_get_dynamic())
        printf("Warning: dynamic adjustment of threads has been set\n");

    omp_set_num_threads(num_threads);
    
    gtcombined_init(num_processes, num_threads);

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

#pragma omp parallel shared(num_threads)
    {
        for (int i = 0; i < num_iter; i++) {
            gtcombined_barrier();
        }
    }

    gettimeofday(&t2, NULL);

    total_time += (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);

    gtcombined_finalize();

    MPI_Finalize();

    printf("Processes: %d\tThreads: %d\tTotal time %f\tAverage time %f\n", num_processes, num_threads, total_time, total_time / num_processes);

    return 0;
}