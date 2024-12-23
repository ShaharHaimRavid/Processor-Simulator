#include <stdio.h>

#include "files.h"

#include "common.h"
#include "cache.h"
#include "registers.h"
#include "instruction_memory.h"
#include "main_memory.h"
#include "core.h"

#define GET_ARG(argc, argv, i, default_val) (argc == 0 ? argv[i] : default_val)

static void parse_args(int argc, char* argv[], core_files_t* core_files0, core_files_t* core_files1, core_files_t* core_files2, core_files_t* core_files3, mem_files_t* mem_files)
{
	core_files0->imem = GET_ARG(argc, argv, 1, "imem0.txt");
	core_files1->imem = GET_ARG(argc, argv, 2, "imem1.txt");
	core_files2->imem = GET_ARG(argc, argv, 3, "imem2.txt");
	core_files3->imem = GET_ARG(argc, argv, 4, "imem3.txt");
	mem_files->memin = GET_ARG(argc, argv, 5, "memin.txt");
	mem_files->memout = GET_ARG(argc, argv, 6, "memout.txt");
	core_files0->regout = GET_ARG(argc, argv, 7, "regout0.txt");
	core_files1->regout = GET_ARG(argc, argv, 8, "regout1.txt");
	core_files2->regout = GET_ARG(argc, argv, 9, "regout2.txt");
	core_files3->regout = GET_ARG(argc, argv, 10, "regout3.txt");
	core_files0->coretrace = GET_ARG(argc, argv, 11, "core0trace.txt");
	core_files1->coretrace = GET_ARG(argc, argv, 12, "core1trace.txt");
	core_files2->coretrace = GET_ARG(argc, argv, 13, "core2trace.txt");
	core_files3->coretrace = GET_ARG(argc, argv, 14, "core3trace.txt");
	mem_files->bustrace = GET_ARG(argc, argv, 15, "bustrace.txt");
	core_files0->dsram = GET_ARG(argc, argv, 16, "dsram0.txt");
	core_files1->dsram = GET_ARG(argc, argv, 17, "dsram1.txt");
	core_files2->dsram = GET_ARG(argc, argv, 18, "dsram2.txt");
	core_files3->dsram = GET_ARG(argc, argv, 19, "dsram3.txt");
	core_files0->tsram = GET_ARG(argc, argv, 20, "tsram0.txt");
	core_files1->tsram = GET_ARG(argc, argv, 21, "tsram1.txt");
	core_files2->tsram = GET_ARG(argc, argv, 22, "tsram2.txt");
	core_files3->tsram = GET_ARG(argc, argv, 23, "tsram3.txt");
	core_files0->stats = GET_ARG(argc, argv, 24, "stats0.txt");
	core_files1->stats = GET_ARG(argc, argv, 25, "stats1.txt");
	core_files2->stats = GET_ARG(argc, argv, 26, "stats2.txt");
	core_files3->stats = GET_ARG(argc, argv, 27, "stats3.txt");
}

int main(int argc, char* argv[])
{
	if (argc != 1 && argc != 28)
	{
		printf("Usage: %s imem0.txt imem1.txt imem2.txt imem3.txt memin.txt memout.txt regout0.txt regout1.txt regout2.txt regout3.txt core0trace.txt core1trace.txt core2trace.txt core3trace.txt bustrace.txt dsram0.txt dsram1.txt dsram2.txt dsram3.txt tsram0.txt tsram1.txt tsram2.txt tsram3.txt stats0.txt stats1.txt stats2.txt stats3.txt\n", argv[0]);
		return 1;
	}

	core_files_t core_files0;
	core_files_t core_files1;
	core_files_t core_files2;
	core_files_t core_files3;
	mem_files_t mem_files;

	parse_args(argc, argv, &core_files0, &core_files1, &core_files2, &core_files3, &mem_files);

	return 0;
}
