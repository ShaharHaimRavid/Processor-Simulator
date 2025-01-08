#include "arbitor.h"
#include "common.h"

void arbitor_on_transaction_start(arbitor_t *arbitor, bus_origid_t origid)
{
    arbitor->is_transaction_open = TRUE;
    arbitor->transaction_origid = origid;
}
void arbitor_on_transaction_end(arbitor_t *arbitor, bus_origid_t origid)
{
    arbitor->is_transaction_open = FALSE;
    for (int i = 0; i < NUM_CORES - 1; i++)
    {
        if (arbitor->prioritized_origids[i] != origid)
        {
            continue;
        }

        for (int j = i; j < NUM_CORES - 1; j++)
        {
            arbitor->prioritized_origids[j] = arbitor->prioritized_origids[j + 1];
        }
        arbitor->prioritized_origids[NUM_CORES - 1] = origid;
        break;
    }
}
int arbitor_prioritize(arbitor_t *arbitor, bus_origid_t *prioritized_origid[])
{
    *prioritized_origid = arbitor->prioritized_origids;
    return NUM_CORES;
}
