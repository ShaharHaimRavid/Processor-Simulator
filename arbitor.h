#ifndef ARBITOR_H
#define ARBITOR_H

#include "main_memory.h"

typedef struct
{
    bool_t is_transaction_open;
    int transaction_core_id;
    int prioritized_core_ids[NUM_CORES];
} arbitor_t;

void arbitor_init(arbitor_t* arbitor);
void arbitor_on_transaction_start(arbitor_t *arbitor, int core_id);
void arbitor_on_transaction_end(arbitor_t *arbitor, int core_id);
int* arbitor_prioritize(arbitor_t *arbitor);

#endif // ARBITOR_H