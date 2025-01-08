#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "instruction_memory.h"
#include "main_memory.h"

int main(int argc, char* argv[]) {
    // Define default file names
    int num_of_files = 27;
    const char* default_files[] = {
        "imem0.txt",
        "imem1.txt",
        "imem2.txt",
        "imem3.txt",
        "memin.txt",
        "memout.txt",
        "regout0.txt",
        "regout1.txt",
        "regout2.txt",
        "regout3.txt",
        "core0trace.txt",
        "core1trace.txt",
        "core2trace.txt",
        "core3trace.txt",
        "bustrace.txt",
        "dsram0.txt",
        "dsram1.txt",
        "dsram2.txt",
        "dsram3.txt",
        "tsram0.txt",
        "tsram1.txt",
        "tsram2.txt",
        "tsram3.txt",
        "stats0.txt",
        "stats1.txt",
        "stats2.txt",
        "stats3.txt",
    };

    // Validate arguments or use default files
    if (argc != 1 && argc != num_of_files + 1) {
        return EXIT_FAILURE;
    }

    // Determine file names to use
    const char* files[num_of_files];
    if (argc == 1) {
        // Use default file names
        for (int i = 0; i < num_of_files; i++) {
            files[i] = default_files[i];
        }
    }
    else {
        // Use provided file names
        for (int i = 0; i < num_of_files; i++) {
            files[i] = argv[i + 1];
        }
    }

    // Open instruction memory files
    FILE* imem_files[4];
    for (int i = 0; i < 4; i++) {
        imem_files[i] = fopen(files[i], "r");
        if (!imem_files[i]) {
            fprintf(stderr, "Error: Unable to open file %s\n", files[i]);
            // Close any already opened files before exiting
            for (int j = 0; j < i; j++) {
                fclose(imem_files[j]);
            }
            return EXIT_FAILURE;
        }
    }

    // Open main memory file
    FILE* memin_file = fopen(files[4], "r");
    if (!memin_file) {
        fprintf(stderr, "Error: Unable to open file %s\n", files[4]);
        // Close instruction memory files before exiting
        for (int i = 0; i < 4; i++) {
            fclose(imem_files[i]);
        }
        return EXIT_FAILURE;
    }

    // Open registers files
    FILE* regout_files[4];
    for (int i = 0; i < 4; i++) {
        regout_files[i] = fopen(files[6 + i], "w");
        if (!regout_files[i]) {
            fprintf(stderr, "Error: Unable to open file %s\n", files[6 + i]);
            for (int j = 0; j < i; j++) {
                fclose(regout_files[j]);
            }
            return EXIT_FAILURE;
        }
    }

    // Open DSRAM and TSRAM files
    FILE* dsram_files[4];
    FILE* tsram_files[4];
    for (int i = 0; i < 4; i++) {
        dsram_files[i] = fopen(files[16 + i], "w");
        if (!dsram_files[i]) {
            fprintf(stderr, "Error: Unable to open file %s\n", files[16 + i]);
            for (int j = 0; j < i; j++) {
                fclose(dsram_files[j]);
            }
            return EXIT_FAILURE;
        }

        tsram_files[i] = fopen(files[20 + i], "w");
        if (!tsram_files[i]) {
            fprintf(stderr, "Error: Unable to open file %s\n", files[20 + i]);
            for (int j = 0; j < i; j++) {
                fclose(tsram_files[j]);
            }
            fclose(dsram_files[i]);
            return EXIT_FAILURE;
        }
    }

    // Initialize instruction memory structures (File -> Struct)
    instruction_memory_t imem_structs[4];
    for (int i = 0; i < 4; i++) {
        instruction_memory_load(&imem_structs[i], imem_files[i]);
        fclose(imem_files[i]); // Close each file after loading
    }

    // Initialize main memory structure (File -> Struct)
    main_memory_t main_mem;
    main_memory_load(&main_mem, memin_file);
    fclose(memin_file); // Close memin file after loading

    // Initialize cores and their registers
    core_t cores[4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 16; j++) {
            cores[i].registers[j] = 0; // Initialize registers to 0 for simplicity
        }
    }

    // Initialize caches and assign them to cores
    cache_t caches[4];
    for (int i = 0; i < 4; i++) {
        cores[i].cache = &caches[i];
    }

    // The rest of the simulation code would go here

    // Call `registers_save` for each core and save output (Struct -> File)
    for (int i = 0; i < 4; i++) {
        registers_save(&cores[i], regout_files[i]);
        fclose(regout_files[i]);
    }

    // Print memory to file (Structure -> File)
    main_memory_save(&main_mem, memout_file);
    fclose(memout_file);

    // Call `registers_save` for each core and save output (Struct -> File)
    for (int i = 0; i < 4; i++) {
        registers_save(&cores[i], regout_files[i]);
        fclose(regout_files[i]);
    }

    // Save cache data (DSRAM) and metadata (TSRAM) to files (Struct -> File)
    for (int i = 0; i < 4; i++) {
        dsram_save(&caches[i], dsram_files[i]);
        tsram_save(&caches[i], tsram_files[i]);
        fclose(dsram_files[i]);
        fclose(tsram_files[i]);
    }

    return EXIT_SUCCESS;
}
