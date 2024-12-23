#pragma once

#include "common.h"

typedef struct {
	uint32_t data[1<<20];
} main_memory_t;

typedef struct {
	uint32_t temp;
} main_memory_bus_t;

void main_memory_read(main_memory_bus_t* bus, uint32_t addr, uint32_t *data);
void main_memory_write(main_memory_bus_t* bus, uint32_t addr, uint32_t data);