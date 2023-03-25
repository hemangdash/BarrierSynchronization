#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include "gtmpi.h"

// Tournament Barrier

static int P;

void runRound(int p_num, int rounds, int current_round) {
    if (current_round == ceil(log2(P))) {
        return;
    }

    MPI_Status status;
    if (!(p_num % (int) pow(2, current_round + 1))) {
        int peer = pow(2, current_round) + p_num;

        if (peer < P) {
            MPI_Recv(NULL, 0, MPI_INT, peer, 1, MPI_COMM_WORLD, &status);
        }

        runRound(p_num, rounds, current_round + 1);

        if (peer < P) {
            MPI_Send(NULL, 0, MPI_INT, peer, 1, MPI_COMM_WORLD);
        }
    } else {
        int peer = p_num - pow(2, current_round);

        if (peer >= 0) {
            MPI_Send(NULL, 0, MPI_INT, peer, 1, MPI_COMM_WORLD);
            MPI_Recv(NULL, 0, MPI_INT, peer, 1, MPI_COMM_WORLD, &status);
        }
    }
}

void gtmpi_init(int num_processes) {
    P = num_processes;
}

void gtmpi_barrier() {
    int p_num;

    MPI_Comm_rank(MPI_COMM_WORLD, &p_num);

    int max_rounds = ceil(log2(P));
    for (int rounds = 1; rounds <= max_rounds; rounds++) {
        if (p_num % (int) pow(2, rounds)) {
            max_rounds = rounds;
        }
    }

    runRound(p_num, max_rounds, 0);
}

void gtmpi_finalize() {

}
