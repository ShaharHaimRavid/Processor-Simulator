#include "main_memory.h"

void main_memory_bus_snoop_observe(main_memory_bus_t *bus, bus_origid_t id, bus_snoop_cb_t cb, void *user_data)
{
	bus->observers[id] = cb;
	bus->observers_data[id] = user_data;
}
void main_memory_read(main_memory_bus_t* bus, bus_addr_t addr, bus_command_t cmd, block* data, bool_t* shared){}
void main_memory_write(main_memory_bus_t* bus, bus_addr_t addr, block data){}

void main_memory_load(main_memory_t* mem, FILE* memin) {
    char line[9]; // To store a line of 8 hex digits plus the null terminator
    int i = 0;

    // Initialize the data array to 0
    for (i = 0; i < (1 << 20); i++) {
        mem->data[i] = 0;
    }

    // Reset index for reading file
    i = 0;

    // Read lines from the file and populate the data array
    while (i < (1 << 20) && fgets(line, sizeof(line), memin)) {
        mem->data[i] = (uint32_t)strtol(line, NULL, 16);
        i++;
    }
}

void main_memory_save(main_memory_t* mem, FILE* memout) {
    // Start from the first line and go up to the last non-zero line
    uint32_t last_non_zero = 0;

    // Find the last non-zero index
    for (uint32_t i = 0; i < (1 << 20); i++) {
        if (mem->data[i] != 0) {
            last_non_zero = i;
        }
    }

    // Print all lines up to the last non-zero line
    for (uint32_t i = 0; i <= last_non_zero; i++) {
        fprintf(memout, "%08X\n", mem->data[i]);
    }
}
