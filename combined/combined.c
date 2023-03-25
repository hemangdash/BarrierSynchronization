#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include "combined.h"

// Combined barrier: Dissemination barrier and Sense-reversing centralized barrier

static int P;

static int cnt;
static bool s;

typedef union local_sense {
    bool lsense;
} local_sense_t;

static local_sense_t *s_list = NULL;

void gtcombined_init(int num_processes, int num_threads) {
    P = num_processes;

    s = true;
    cnt = num_threads;

    if (s_list == NULL) {
        s_list = (local_sense_t *) malloc(num_threads * sizeof(local_sense_t));
    }

    for (int i = 0; i < num_threads; i++) {
        s_list[i].lsense = true;
    }
}

void gtcombined_barrier() {
    int thread_count = omp_get_num_threads();
    int thread_num = omp_get_thread_num();

    s_list[thread_num].lsense = !(s_list[thread_num].lsense);

    if (__sync_fetch_and_sub(&cnt, 1) == 1) {
        // Dissemination Barrier
        int vpid;
    
        MPI_Comm_rank(MPI_COMM_WORLD, &vpid);

        int rounds = ceil(log2(P));

        for (int i = 0; i < rounds; i++) {
            int destination = ((int) pow(2, i) + vpid) % P;
            MPI_Send(NULL, 0, MPI_INT, destination, 1, MPI_COMM_WORLD);
            MPI_Status status;
            int src = vpid - pow(2, i);
            while (src < 0) {
                src = (P + src) % P;
            }
            MPI_Recv(NULL, 0, MPI_INT, src, 1, MPI_COMM_WORLD, &status);
        }

        s = s_list[thread_num].lsense;
        cnt = thread_count;
    }
    else {
        while (s_list[thread_num].lsense != s);
    }
}

void gtcombined_finalize() {
    free(s_list);
}