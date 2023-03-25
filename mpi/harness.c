#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "gtmpi.h"
#include <sys/time.h>

int main(int argc, char** argv)
{
    int num_processes, num_rounds = 1;

    double total_time = 0.0;

    MPI_Init(&argc, &argv);
    
    if (argc < 2) {
        fprintf(stderr, "Usage: ./harness [NUM_PROCS]\n");
        exit(EXIT_FAILURE);
    }

    num_processes = strtol(argv[1], NULL, 10);

    gtmpi_init(num_processes);

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    
    int k;
    for(k = 0; k < num_rounds; k++) {
        gtmpi_barrier();
    }

    gettimeofday(&t2, NULL);

    total_time += (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);

    gtmpi_finalize();

    MPI_Finalize();

    printf("Number of Processes: %d\tTotal time %f\tAverage time %f\n", num_processes, total_time, total_time / num_processes);

    return 0;
}
