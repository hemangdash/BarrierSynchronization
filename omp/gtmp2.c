#include <omp.h>
#include "gtmp.h"
#include <stdlib.h>
#include <stdbool.h>

// Tree-based Barrier

typedef struct treenode {
    bool parent_sense;
    bool *parent_ptr;
    bool *child_ptrs[2];
    bool have_child[4];
    bool child_not_ready[4];
    bool dummy;
} treenode;

static treenode *nodes;
static bool *senses;
static int P;

void gtmp_init(int num_threads) {
    P = num_threads;

    nodes = (treenode *) malloc(num_threads * sizeof(treenode));
    senses = (bool *) malloc(num_threads * sizeof(bool));

    for (int i = 0; i < num_threads; i++) {
        nodes[i].parent_sense = false;

        for (int j = 0; j < 4; j++) {
            if (((4 * i) + j) < num_threads - 1) {
                nodes[i].have_child[j] = true;
            } else {
                nodes[i].have_child[j] = false;
            }
        }

        for (int j = 0; j < 4; j++) {
            nodes[i].child_not_ready[j] = nodes[i].have_child[j];
        }

        if (i == 0) {
            nodes[i].parent_ptr = &(nodes[i].dummy);
        } else {
            nodes[i].parent_ptr = &(nodes[(int)((i - 1) / 4)].child_not_ready[(i - 1) % 4]);
        }

        if (2 * i + 1 >= num_threads) {
            nodes[i].child_ptrs[0] = &(nodes[i].dummy);
        } else {
            nodes[i].child_ptrs[0] = &(nodes[i * 2 + 1].parent_sense);
        }

        if (2 * i + 2 >= num_threads) {
            nodes[i].child_ptrs[1] = &(nodes[i].dummy);
        } else {
            nodes[i].child_ptrs[1] = &(nodes[i * 2 + 2].parent_sense);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        senses[i] = true;
    }
}

void gtmp_barrier() {
    int thread_num = omp_get_thread_num();

    while(
        nodes[thread_num].child_not_ready[3]
        || nodes[thread_num].child_not_ready[2]
        || nodes[thread_num].child_not_ready[1]
        || nodes[thread_num].child_not_ready[0]
        );

    for (int j = 0; j < 4; j++) {
        nodes[thread_num].child_not_ready[j] = nodes[thread_num].have_child[j];
    }

    *(nodes[thread_num].parent_ptr) = false;

    if (thread_num != 0) {
        while (nodes[thread_num].parent_sense != senses[thread_num]);
    }

    *(nodes[thread_num].child_ptrs[0]) = senses[thread_num];
    *(nodes[thread_num].child_ptrs[1]) = senses[thread_num];
    senses[thread_num] = !(senses[thread_num]);
}

void gtmp_finalize() {
    free(nodes);
    free(senses);
}

