#include "main_memory.h"

void main_memory_write(main_memory_t *mem, bus_addr_t addr, block data)
{
    // Copy the data from the block to the memory
    for (int i = 0; i < 4; i++)
    {
        mem->data[addr + i] = data[i];
    }
}
void main_memory_read(main_memory_t *mem, bus_addr_t addr, block *data)
{
    // Copy the data from the memory to the block
    for (int i = 0; i < 4; i++)
    {
        (*data)[i] = mem->data[addr + i];
    }
}

void main_memory_bus_snoop_observe(main_memory_bus_t *bus, bus_origid_t id, bus_snoop_cb_t cb, void *user_data)
{
    bus->observers[id] = cb;
    bus->observers_data[id] = user_data;
}

void main_memory_bus_init(main_memory_bus_t *bus, FILE *bustrace, main_memory_t *mem)
{
    bus->bustrace_file = bustrace;
    bus->memory = *mem;
    mem->bus_data = bus;
    bus->transaction_open = FALSE;
    bus->flush_count = 0;
    bus->transaction_origid = BUS_ORIGID_MAIN_MEMORY;
}

void main_memory_init(main_memory_t *mem)
{
    mem->data = (uint32_t *)calloc(1 << 20, sizeof(uint32_t));
    mem->latency_cycles = 0;
    mem->pending_addr = 0;
    mem->transaction_pending = FALSE;
}

void main_memory_free(main_memory_t *mem)
{
    free(mem);
}

void main_memory_load(main_memory_t *mem, FILE *memin)
{
    char line[9]; // To store a line of 8 hex digits plus the null terminator
    int i = 0;

    // Initialize the data array to 0
    for (i = 0; i < (1 << 20); i++)
    {
        mem->data[i] = 0;
    }

    // Reset index for reading file
    i = 0;

    // Read lines from the file and populate the data array
    while (i < (1 << 20) && fgets(line, sizeof(line), memin))
    {
        mem->data[i] = (uint32_t)strtol(line, NULL, 16);
        i++;
    }
}
void main_memory_save(main_memory_t *mem, FILE *memout)
{
    // Start from the first line and go up to the last non-zero line
    uint32_t last_non_zero = 0;

    // Find the last non-zero index
    for (uint32_t i = 0; i < (1 << 20); i++)
    {
        if (mem->data[i] != 0)
        {
            last_non_zero = i;
        }
    }

    // Print all lines up to the last non-zero line
    for (uint32_t i = 0; i <= last_non_zero; i++)
    {
        fprintf(memout, "%08X\n", mem->data[i]);
    }
}

bool_t main_memory_bus_action(main_memory_bus_t *bus, bus_origid_t id, bus_addr_t addr, bus_command_t cmd, word *data, bool_t *shared)
{
    if (bus->transaction_open)
    {
        return FALSE;
    }

    // Notify the observers
    for (int i = 0; i < 4; i++)
    {
        if (bus->observers[i] != NULL)
        {
            bus->observers[i](id, cmd, addr, 0, shared, bus->observers_data[i]);
        }
    }

    if (id == BUS_ORIGID_MAIN_MEMORY)
    {
        return TRUE;
    }
    if (cmd != BUS_COMMAND_READ && cmd != BUS_COMMAND_READX)
    {
        return FALSE;
    }

    bus->transaction_open = TRUE;
    bus->transaction_origid = id;
    bus->flush_count = 0;

    // set main memory to return data
    bus->memory.latency_cycles = 16;
    bus->memory.pending_addr = addr & 0xFFFFFFFC; // set lower 2 bit to zero to get the block address at first word
    bus->memory.transaction_pending = TRUE;
}

bool_t main_memory_bus_write(main_memory_bus_t *bus, bus_addr_t addr, block data)
{
    main_memory_write(&bus->memory, addr, data);
    return TRUE;
}

void main_memory_clk(main_memory_t *mem)
{
    if (!mem->transaction_pending)
    {
        return;
    }

    if (mem->latency_cycles > 0)
    {
        mem->latency_cycles--;
        return;
    }
    bool_t shared;
    main_memory_bus_t *bus = (main_memory_bus_t *)mem->bus_data;
    main_memory_bus_action(bus, BUS_ORIGID_MAIN_MEMORY, mem->pending_addr, BUS_COMMAND_FLUSH, mem->data[mem->pending_addr], &shared);

    mem->pending_addr++;            // increment to next word
    if (mem->pending_addr % 4 == 0) // last word of block
    {
        mem->transaction_pending = FALSE;
    }
}
