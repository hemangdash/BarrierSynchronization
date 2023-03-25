#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include "gtmpi.h"

// Dissemination barrier

static int P;

void gtmpi_init(int num_processes) {
    P = num_processes;
}

void gtmpi_barrier() {
    int p_num;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &p_num);

    int rounds = ceil(log2(P));

    for (int i = 0; i < rounds; i++) {
        int destination = ((int) pow(2, i) + p_num) % P;
        MPI_Send(NULL, 0, MPI_INT, destination, 1, MPI_COMM_WORLD);
        MPI_Status status;
        int src = p_num - pow(2, i);
        while (src < 0) {
            src = (P + src) % P;
        }
        MPI_Recv(NULL, 0, MPI_INT, src, 1, MPI_COMM_WORLD, &status);
    }
}

void gtmpi_finalize() {

}
