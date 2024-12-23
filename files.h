#pragma once

#include <stdio.h>

// a struct for the input/output files for a single core
typedef struct 
{
	char* imem;
	char* regout;
	char* coretrace;
	char* dsram;
	char* tsram;
	char* stats;
} core_files_t;

// a struct for the input/output files for the memory
typedef struct 
{
	char* memin;
	char* memout;
	char* bustrace;
} mem_files_t;

void dump_imem(FILE* f);
void dump_regout(FILE* f);
void dump_coretrace(FILE* f);
void dump_dsram(FILE* f);
void dump_tsram(FILE* f);
void dump_stats(FILE* f);
void dump_memin(FILE* f);
void dump_memout(FILE* f);
void dump_bustrace(FILE* f);