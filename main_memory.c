#include "main_memory.h"

void main_memory_bus_snoop_observe(main_memory_bus_t *bus, bus_origid_t id, bus_snoop_cb_t cb, void *user_data)
{
	bus->observers[id] = cb;
	bus->observers_data[id] = user_data;
}
void main_memory_read(main_memory_bus_t* bus, bus_addr_t addr, bus_command_t cmd, block* data, bool_t* shared){}
void main_memory_write(main_memory_bus_t* bus, bus_addr_t addr, block data){}