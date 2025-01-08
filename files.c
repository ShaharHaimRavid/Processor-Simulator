#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "files.h"

bool_t core_files_open(core_files_t *core_files)
{
    core_files->imem = fopen(core_files->imem_path, "r");
    if (core_files->imem == NULL)
    {
        goto fail_imem;
    }
    core_files->regout = fopen(core_files->regout_path, "w");
    if (core_files->regout == NULL)
    {
        goto fail_regout;
    }
    core_files->coretrace = fopen(core_files->coretrace_path, "w");
    if (core_files->coretrace == NULL)
    {
        goto fail_coretrace;
    }
    core_files->dsram = fopen(core_files->dsram_path, "w");
    if (core_files->dsram == NULL)
    {
        goto fail_dsram;
    }
    core_files->tsram = fopen(core_files->tsram_path, "w");
    if (core_files->tsram == NULL)
    {
        goto fail_tsram;
    }
    core_files->stats = fopen(core_files->stats_path, "w");
    if (core_files->stats == NULL)
    {
        goto fail_stats;
    }
    return TRUE;

fail_stats:
    fclose(core_files->tsram);
fail_tsram:
    fclose(core_files->dsram);
fail_dsram:
    fclose(core_files->coretrace);
fail_coretrace:
    fclose(core_files->regout);
fail_regout:
    fclose(core_files->imem);
fail_imem:
    return FALSE;
}

bool_t core_files_close(core_files_t *core_files)
{
    fclose(core_files->imem);
    fclose(core_files->regout);
    fclose(core_files->coretrace);
    return TRUE;
}

bool_t mem_files_open(mem_files_t *mem_files)
{
    mem_files->memin = fopen(mem_files->memin_path, "r");
    if (mem_files->memin == NULL)
    {
        goto fail_memin;
    }
    mem_files->memout = fopen(mem_files->memout_path, "w");
    if (mem_files->memout == NULL)
    {
        goto fail_memout;
    }
    mem_files->bustrace = fopen(mem_files->bustrace_path, "w");
    if (mem_files->bustrace == NULL)
    {
        goto fail_bustrace;
    }
    return TRUE;

fail_bustrace:
    fclose(mem_files->memout);
fail_memout:
    fclose(mem_files->memin);
fail_memin:
    return FALSE;
}

bool_t mem_files_close(mem_files_t *mem_files)
{
    fclose(mem_files->memin);
    fclose(mem_files->memout);
    fclose(mem_files->bustrace);
    return TRUE;
}