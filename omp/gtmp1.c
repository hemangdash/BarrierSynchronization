#include <omp.h>
#include "gtmp.h"
#include <stdlib.h>
#include <stdbool.h>

// Sense-reversing centralized barrier

static int cnt;
static bool s;

typedef union local_sense {
    bool lsense;
} local_sense_t;

static local_sense_t *s_list = NULL;

void gtmp_init(int num_threads) {
    s = true;
    cnt = num_threads;

    if (s_list == NULL) {
        s_list = (local_sense_t *) malloc(num_threads * sizeof(local_sense_t));
    }

    for (int i = 0; i < num_threads; i++) {
        s_list[i].lsense = true;
    }
}

void gtmp_barrier() {
    int thread_count = omp_get_num_threads();
    int thread_num = omp_get_thread_num();

    s_list[thread_num].lsense = !(s_list[thread_num].lsense);

    if (__sync_fetch_and_sub(&cnt, 1) == 1) {
        s = s_list[thread_num].lsense;
        cnt = thread_count;
    }
    else {
        while(s_list[thread_num].lsense != s);
    }
}

void gtmp_finalize() {
    free(s_list);
}