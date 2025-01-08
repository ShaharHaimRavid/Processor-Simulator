#ifndef CORE_H
#define CORE_H

#include "files.h"
#include "cache.h"
#include "instruction_memory.h"
#include "main_memory.h"
#include "common.h"

typedef enum
{
	OPCODE_ADD = 0,
	OPCODE_SUB = 1,
	OPCODE_AND = 2,
	OPCODE_OR = 3,
	OPCODE_XOR = 4,
	OPCODE_MUL = 5,
	OPCODE_SLL = 6,
	OPCODE_SRA = 7,
	OPCODE_SRL = 8,
	OPCODE_BEQ = 9,
	OPCODE_BNE = 10,
	OPCODE_BLT = 11,
	OPCODE_BGT = 12,
	OPCODE_BLE = 13,
	OPCODE_BGE = 14,
	OPCODE_JAL = 15,
	OPCODE_LW = 16,
	OPCODE_SW = 17,
	OPCODE_HALT = 20
} OPCODES;

typedef struct
{
	uint16_t pc;
	uint32_t instruction;

	uint32_t stalls;
} instruction_fetch_state;

typedef struct
{
	uint16_t pc;
	uint32_t instruction;

	uint8_t opcode;
	uint8_t rs;
	uint8_t rt;
	uint8_t rd;
	uint16_t imm;

	uint32_t stalls;
} instruction_decode_state;

typedef struct
{
	uint32_t do_work;
	uint16_t pc;
	uint32_t instruction;

	OPCODES opcode;
	uint32_t rsv;
	uint32_t rtv;
	uint8_t rd;
	uint32_t rdv;
	uint16_t imm;

	uint32_t alu_result;
} execute_state;

typedef enum
{
	MEM_ACCESS_NONE,
	MEM_ACCESS_READ,
	MEM_ACCESS_WRITE
} MEM_ACCESS_STATE;

typedef struct
{
	uint32_t do_work;
	bool_t action_success;
	uint16_t pc;
	uint32_t instruction;

	OPCODES opcode;
	MEM_ACCESS_STATE state;
	uint32_t rtv;
	uint8_t rd;
	uint32_t rdv;
	uint32_t alu_result;
} memory_access_state;

typedef struct
{
	uint32_t do_work;
	uint16_t pc;
	uint32_t instruction;

	uint8_t rd;
	uint32_t mem_data;
} write_back_state;

typedef struct
{
	uint64_t cycles_count;
	uint64_t instructions_exe_count;
	uint64_t decode_stall_count;
	uint64_t mem_stall_count;

	bool_t halted;

	core_files_t *files;
	instruction_memory_t *imem;
	cache_t *cache;
	uint32_t registers[16];

	// pipeline states:
	instruction_fetch_state fetch;
	instruction_decode_state decode;
	execute_state execute;
	memory_access_state memory_access;
	write_back_state write_back;
} core_t;

void register_write(uint32_t *regs, uint16_t addr, uint32_t data);
uint32_t register_read(uint32_t *regs, uint16_t addr);

void core_clk(core_t *core);
bool_t is_halted(core_t *core)
{
	return core->halted;
}

void core_init(core_t *core, core_files_t *files, cache_t *cache);
void core_save(core_t *core);
void core_free(core_t *core);

void core_instruction_fetch(core_t *core);
void core_instruction_decode(core_t *core);
void core_execute(core_t *core);
void core_memory_access(core_t *core);
void core_write_back(core_t *core);

#endif // CORE_H