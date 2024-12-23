#define _CRT_SECURE_NO_WARNINGS

#include "instruction_memory.h"

uint32_t  instruction_memory_read(instruction_memory_t* imem, uint32_t addr)
{
	return imem->instructions[addr];
}

void instruction_memory_load(instruction_memory_t* mem, FILE* imem)
{
	// each entry in the instruction set is a 32bit hexadecimal number (8 characters)
	for (int i = 0; i < 1024; i++)
		fscanf(imem, "%x", &mem->instructions[i]);
}