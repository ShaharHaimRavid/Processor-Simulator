#ifndef INSTRUCTION_MEMORY_H
#define INSTRUCTION_MEMORY_H

#include "common.h"
#include <stdio.h>

typedef struct
{
	uint32_t instructions[1024];
} instruction_memory_t;

uint32_t instruction_memory_read(instruction_memory_t *imem, uint32_t addr);
void instruction_memory_load(instruction_memory_t *mem, FILE *imem); // load instructions from file

#endif // INSTRUCTION_MEMORY_H