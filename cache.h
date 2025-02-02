#ifndef CACHE_H
#define CACHE_H

#include "main_memory.h"
#include "common.h"

typedef enum
{
	MESI_INVALID = 0,
	MESI_SHARED = 1,
	MESI_EXCLUSIVE = 2,
	MESI_MODIFIED = 3
} MESI;

typedef struct
{
	uint64_t read_hit_count;
	uint64_t read_miss_count;
	uint64_t write_hit_count;
	uint64_t write_miss_count;

	block data[64];
	uint16_t metadata[64];
	uint8_t next_way_by_set[16];

	bus_origid_t id;
	main_memory_bus_t *bus;

	uint32_t pending_addr;
} cache_t;

void cache_init(cache_t *c, main_memory_bus_t *bus, int8_t id);
bool_t cache_read(cache_t *c, uint32_t addr, uint32_t *data);
bool_t cache_write(cache_t *c, uint32_t addr, uint32_t data);
void cache_flush_all(cache_t* c);
void dsram_save(cache_t *c, FILE *dsram);
void tsram_save(cache_t *c, FILE *tsram);

#endif // CACHE_H