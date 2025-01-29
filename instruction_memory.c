#define _CRT_SECURE_NO_WARNINGS

#include "instruction_memory.h"
#include <string.h>
#include <stdlib.h>

uint32_t instruction_memory_read(instruction_memory_t *imem, uint32_t addr)
{
    return imem->instructions[addr];
}

void instruction_memory_load(instruction_memory_t *mem, FILE *imem)
{
    char line[9]; // To store a line of 8 hex digits plus the null terminator

    // Initialize the instructions array to 0
    for (int i = 0; i < 1024; i++)
    {
        mem->instructions[i] = 0;
    }

    // Read lines from the file and populate the instructions array
    for (int i = 0; i < 1024 && fgets(line, sizeof(line), imem); i++)
    {
        // Strip any unwanted character
        line[strcspn(line, "\r\n")] = '\0';

        // Skip empty lines
        if (line[0] == '\0')
        {
            continue;
        }
        mem->instructions[i] = (uint32_t)strtol(line, NULL, 16);
    }

    printf("Instructions memory:\n");
    for (int j = 0; j < 15; j++)
    {
        printf("% 08x\n", mem->instructions[j]);
    }
}