#define _CRT_SECURE_NO_WARNINGS

#include "instruction_memory.h"

uint32_t  instruction_memory_read(instruction_memory_t* imem, uint32_t addr)
{
	return imem->instructions[addr];
}

/*
void instruction_memory_load(instruction_memory_t* mem, FILE* imem)
{
	// each entry in the instruction set is a 32bit hexadecimal number (8 characters)
	for (int i = 0; i < 1024; i++)
		fscanf(imem, "%x", &mem->instructions[i]);
}
*/

void instruction_memory_load(instruction_memory_t* mem, FILE* imem) {
    char line[9]; // To store a line of 8 hex digits plus the null terminator
    int i = 0;

    // Initialize the instructions array to 0
    for (i = 0; i < 1024; i++) {
        mem->instructions[i] = 0;
    }

    // Reset index for reading file
    i = 0;

    // Read lines from the file and populate the instructions array
    while (i < 1024 && fgets(line, sizeof(line), imem)) {
        mem->instructions[i] = (uint32_t)strtol(line, NULL, 16);
        i++;
    }
}