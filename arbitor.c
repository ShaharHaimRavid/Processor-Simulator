#include "arbitor.h"
#include "common.h"

void arbitor_on_transaction_start(core_arbitor_t *arbitor, int core_id)
{
    arbitor->is_transaction_open = TRUE;
    arbitor->transaction_core_id = core_id;
}
void arbitor_on_transaction_end(core_arbitor_t *arbitor)
{
    arbitor->is_transaction_open = FALSE;
    for (int i = 0; i < NUM_CORES - 1; i++)
    {
        if (arbitor->prioritized_core_ids[i] != arbitor->transaction_core_id)
        {
            continue;
        }

        for (int j = i; j < NUM_CORES - 1; j++)
        {
            arbitor->prioritized_core_ids[j] = arbitor->prioritized_core_ids[j + 1];
        }
        arbitor->prioritized_core_ids[NUM_CORES - 1] = arbitor->transaction_core_id;
        break;
    }
}
int *arbitor_prioritize(core_arbitor_t *arbitor)
{
    return arbitor->prioritized_core_ids;
}
void arbitor_init(core_arbitor_t *arbitor)
{
    arbitor->transaction_core_id = 0;
    arbitor->is_transaction_open = FALSE;
    for (int i = 0; i < NUM_CORES; i++)
    {
        arbitor->prioritized_core_ids[i] = i;
    }
}