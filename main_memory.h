#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#include <stdio.h>
#include "common.h"
#include "arbitor.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef uint32_t word;
typedef word block[4];

typedef enum
{
	BUS_ORIGID_CORE0 = 0,
	BUS_ORIGID_CORE1 = 1,
	BUS_ORIGID_CORE2 = 2,
	BUS_ORIGID_CORE3 = 3,
	BUS_ORIGID_MAIN_MEMORY = 4
} bus_origid_t;

typedef enum
{
	BUS_COMMAND_NONE = 0,
	BUS_COMMAND_READ = 1,
	BUS_COMMAND_READX = 2,
	BUS_COMMAND_FLUSH = 3
} bus_command_t;

typedef uint32_t bus_addr_t;

typedef void(bus_snoop_cb_t)(bus_origid_t, bus_command_t, bus_addr_t, uint32_t, bool_t *, void *);
typedef bool_t(bus_action_cb_t)(bus_addr_t, bus_command_t, block *, bool_t *, void *);

typedef struct
{
	word *data;
	bus_action_cb_t *bus_action;
	void *bus_data;
} main_memory_t;

typedef struct
{
	bus_snoop_cb_t *observers[4];
	void *observers_data[4];

	main_memory_t memory;
	FILE *bustrace_file;

	bool_t transaction_open;
	uint16_t flush_count;
	bus_origid_t transaction_origid;
} main_memory_bus_t;

bool_t main_memory_bus_action(main_memory_bus_t *bus, bus_addr_t addr, bus_command_t cmd, block *data, bool_t *shared);
bool_t main_memory_bus_write(main_memory_bus_t *bus, bus_addr_t addr, block data);

void main_memory_bus_init(main_memory_bus_t *bus, FILE *bustrace, main_memory_t *mem);
void main_memory_init(main_memory_t *mem);				// initialize memory to 0
void main_memory_free(main_memory_t *mem);				// free memory
void main_memory_load(main_memory_t *mem, FILE *memin); // load data from file
void main_memory_save(main_memory_t *mem, FILE *memin); // save data to file

void main_memory_bus_snoop_observe(main_memory_bus_t *bus, bus_origid_t id, bus_snoop_cb_t cb, void *user_data);

#endif // MAIN_MEMORY_H