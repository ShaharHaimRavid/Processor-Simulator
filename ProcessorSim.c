#include <stdio.h>

#include "files.h"

#include "common.h"
#include "cache.h"
#include "instruction_memory.h"
#include "main_memory.h"
#include "core.h"
#include "arbitor.h"

#define GET_ARG(argc, argv, i, default_val) (argc == 0 ? argv[i] : default_val);

static void parse_args(int argc, char *argv[], core_files_t *core_files, mem_files_t *mem_files)
{
	core_files[0].imem_path = GET_ARG(argc, argv, 1, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\imem0.txt");
	core_files[1].imem_path = GET_ARG(argc, argv, 2, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\imem1.txt");
	core_files[2].imem_path = GET_ARG(argc, argv, 3, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\imem2.txt");
	core_files[3].imem_path = GET_ARG(argc, argv, 4, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\imem3.txt");
	mem_files->memin_path = GET_ARG(argc, argv, 5, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\memin.txt");
	mem_files->memout_path = GET_ARG(argc, argv, 6, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\memout.txt");
	core_files[0].regout_path = GET_ARG(argc, argv, 7, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\regout0.txt");
	core_files[1].regout_path = GET_ARG(argc, argv, 8, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\regout1.txt");
	core_files[2].regout_path = GET_ARG(argc, argv, 9, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\regout2.txt");
	core_files[3].regout_path = GET_ARG(argc, argv, 10, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\regout3.txt");
	core_files[0].coretrace_path = GET_ARG(argc, argv, 11, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\core0trace.txt");
	core_files[1].coretrace_path = GET_ARG(argc, argv, 12, "core1trace.txt");
	core_files[2].coretrace_path = GET_ARG(argc, argv, 13, "core2trace.txt");
	core_files[3].coretrace_path = GET_ARG(argc, argv, 14, "core3trace.txt");
	mem_files->bustrace_path = GET_ARG(argc, argv, 15, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\bustrace.txt");
	core_files[0].dsram_path = GET_ARG(argc, argv, 16, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\dsram0.txt");
	core_files[1].dsram_path = GET_ARG(argc, argv, 17, "dsram1.txt");
	core_files[2].dsram_path = GET_ARG(argc, argv, 18, "dsram2.txt");
	core_files[3].dsram_path = GET_ARG(argc, argv, 19, "dsram3.txt");
	core_files[0].tsram_path = GET_ARG(argc, argv, 20, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\tsram0.txt");
	core_files[1].tsram_path = GET_ARG(argc, argv, 21, "tsram1.txt");
	core_files[2].tsram_path = GET_ARG(argc, argv, 22, "tsram2.txt");
	core_files[3].tsram_path = GET_ARG(argc, argv, 23, "tsram3.txt");
	core_files[0].stats_path = GET_ARG(argc, argv, 24, "C:\\Users\\shaha\\OneDrive\\מסמכים\\ProcessorProject\\ProcessorSim\\example_files\\stats0.txt");
	core_files[1].stats_path = GET_ARG(argc, argv, 25, "stats1.txt");
	core_files[2].stats_path = GET_ARG(argc, argv, 26, "stats2.txt");
	core_files[3].stats_path = GET_ARG(argc, argv, 27, "stats3.txt");
}

int main(int argc, char *argv[])
{
	if (argc != 1 && argc != 28)
	{
		printf("Usage: %s imem0.txt imem1.txt imem2.txt imem3.txt memin.txt memout.txt regout0.txt regout1.txt regout2.txt regout3.txt core0trace.txt core1trace.txt core2trace.txt core3trace.txt bustrace.txt dsram0.txt dsram1.txt dsram2.txt dsram3.txt tsram0.txt tsram1.txt tsram2.txt tsram3.txt stats0.txt stats1.txt stats2.txt stats3.txt\n", argv[0]);
		return 1;
	}

	core_files_t core_files[4];
	mem_files_t mem_files;

	parse_args(argc, argv, core_files, &mem_files);

	// ========================================================
	//	open input and output files
	// ========================================================
	if (!core_files_open(&core_files[0]))
	{
		fprintf(stderr, "Error: Unable to open core0 files\n");
		return 1;
	}
	if (!core_files_open(&core_files[1]))
	{
		fprintf(stderr, "Error: Unable to open core1 files\n");
		core_files_close(&core_files[0]);
		return 1;
	}
	if (!core_files_open(&core_files[2]))
	{
		fprintf(stderr, "Error: Unable to open core2 files\n");
		core_files_close(&core_files[0]);
		core_files_close(&core_files[1]);
		return 1;
	}
	if (!core_files_open(&core_files[3]))
	{
		fprintf(stderr, "Error: Unable to open core3 files\n");
		core_files_close(&core_files[0]);
		core_files_close(&core_files[1]);
		core_files_close(&core_files[2]);
		return 1;
	}
	if (!mem_files_open(&mem_files))
	{
		fprintf(stderr, "Error: Unable to open memory files\n");
		core_files_close(&core_files[0]);
		core_files_close(&core_files[1]);
		core_files_close(&core_files[2]);
		core_files_close(&core_files[3]);
		return 1;
	}

	// ========================================================
	//	Initialize datastructures
	// ========================================================
	printf("init step\n");
	core_arbitor_t arbitor;
	arbitor_init(&arbitor);

	// Initialize main memory structure (File -> Struct)
	main_memory_t main_mem;
	main_memory_init(&main_mem, &arbitor);
	main_memory_load(&main_mem, mem_files.memin);

	main_memory_bus_t main_mem_bus;
	main_memory_bus_init(&main_mem_bus, mem_files.bustrace, &main_mem, &arbitor);

	// Initialize caches and assign them to cores
	cache_t caches[NUM_CORES];
	for (int i = 0; i < NUM_CORES; i++)
	{
		cache_init(&caches[i], &main_mem_bus, i);
	}
	// Initialize cores and their registers
	core_t cores[NUM_CORES];
	for (int i = 0; i < NUM_CORES; i++)
	{
		core_init(&cores[i], &core_files[i], &caches[i], i);
	}
	// ========================================================
	//	Simulation
	// ========================================================
	printf("simulation step\n");
	int* cored_prioritized;
	while (!is_halted(&cores[0]) || !is_halted(&cores[1]) || !is_halted(&cores[2]) || !is_halted(&cores[3]))
	{

		// we call the core_clk function for each core in the priorized order
		// to verify the correct core gets the memory access transaction
		//core_clk(&cores[0]);

		cored_prioritized = arbitor_prioritize(&arbitor);
		for (int i = 0; i < NUM_CORES; i++)
		{
			core_clk(&cores[cored_prioritized[i]]);
		}

	}
	cored_prioritized = arbitor_prioritize(&arbitor);
	for (int i = 0; i < NUM_CORES; i++)
	{
		core_clk(&cores[cored_prioritized[i]]);
	}
	// ========================================================
	//	Save data to output files
	// ========================================================
	printf("save step\n");
	for (int i = 0; i < NUM_CORES; i++)
	{
		core_save(&cores[i]);
		core_free(&cores[i]);
		core_files_close(&core_files[i]);
	}
	main_memory_save(&main_mem, mem_files.memout);
	printf("4\n");
	main_memory_free(&main_mem);
	mem_files_close(&mem_files);
	printf("finished successfully\n");
	return 0;
}
