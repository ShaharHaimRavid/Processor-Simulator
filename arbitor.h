#ifndef ARBITOR_H
#define ARBITOR_H

#include "main_memory.h"

typedef struct
{
    bool_t is_transaction_open;
    bus_origid_t transaction_origid;

    bus_origid_t prioritized_origids[NUM_CORES];
} arbitor_t;

void arbitor_init(arbitor_t *arbitor)
{
    arbitor->is_transaction_open = FALSE;
    for (int i = 0; i < NUM_CORES; i++)
    {
        arbitor->prioritized_origids[i] = BUS_ORIGID_CORE0 + i;
    }
}
void arbitor_on_transaction_start(arbitor_t *arbitor, bus_origid_t origid);
void arbitor_on_transaction_end(arbitor_t *arbitor, bus_origid_t origid);
int arbitor_prioritize(arbitor_t *arbitor, bus_origid_t *prioritized_origid[]);

#endif // ARBITOR_H