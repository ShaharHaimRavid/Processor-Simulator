#ifndef ARBITOR_H
#define ARBITOR_H

#include "common.h"

typedef struct
{
    bool_t is_transaction_open;
    int transaction_core_id;
    int prioritized_core_ids[NUM_CORES];
} core_arbitor_t;

void arbitor_init(core_arbitor_t *arbitor);
void arbitor_on_transaction_start(core_arbitor_t *arbitor, int core_id);
void arbitor_on_transaction_end(core_arbitor_t *arbitor);
int *arbitor_prioritize(core_arbitor_t *arbitor);

#endif // ARBITOR_H