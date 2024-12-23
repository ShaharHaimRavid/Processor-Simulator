#pragma once

#include "common.h"

typedef enum {
	MESI_INVALID = 0,
	MESI_SHARED = 1,
	MESI_EXCLUSIVE = 2,
	MESI_MODIFIED = 3
} MESI;

typedef struct {
	uint32_t primary[256];
	uint16_t secondary[64];
} cache_t;

uint32_t cache_read(cache_t* c, uint32_t addr);
void cache_write(cache_t* c, uint32_t addr, uint32_t data);
MESI cache_get_MESI(cache_t* c, uint32_t addr, uint16_t* tag_out);