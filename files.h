#pragma once

#include <stdio.h>
#include "common.h"

// a struct for the input/output files for a single core
typedef struct
{
	char *imem_path;
	FILE *imem;
	char *regout_path;
	FILE *regout;
	char *coretrace_path;
	FILE *coretrace;
	char *dsram_path;
	FILE *dsram;
	char *tsram_path;
	FILE *tsram;
	char *stats_path;
	FILE *stats;
} core_files_t;

bool_t core_files_open(core_files_t *core_files);
bool_t core_files_close(core_files_t *core_files);

// a struct for the input/output files for the memory
typedef struct
{
	char *memin_path;
	FILE *memin;
	char *memout_path;
	FILE *memout;
	char *bustrace_path;
	FILE *bustrace;
} mem_files_t;

bool_t mem_files_open(mem_files_t *mem_files);
bool_t mem_files_close(mem_files_t *mem_files);