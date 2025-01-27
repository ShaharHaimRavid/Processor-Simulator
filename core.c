#include "core.h"
#include "files.h"
#include <stdlib.h>

#define INSTRUCTION_4BIT_MASK 0xF
#define INSTRUCTION_8BIT_MASK 0xFF
#define INSTRUCTION_10BIT_MASK 0x3FF
#define INSTRUCTION_12BIT_MASK 0xFFF
#define INSTRUCTION_PARSE_BITS(instruction, offset, mask) ((instruction >> offset) & mask)
bool_t is_halted(core_t *core)
{	
	return core->halted;
}

void register_write(uint32_t *regs, uint16_t addr, uint32_t data)
{
	if (addr == 0)
		return;
	regs[addr] = data;
}

uint32_t register_read(uint32_t *regs, uint16_t addr)
{
	if (addr == 0)
		return 0;
	return regs[addr];
}

void core_save(core_t *core)
{
	printf("core save %d \n", core->id);
	for (int i = 2; i < 16; i++)
	{
		fprintf(core->files->regout, "%08X\n", core->registers[i]);
	}

	dsram_save(core->cache, core->files->dsram);
	tsram_save(core->cache, core->files->tsram);

	fprintf(core->files->stats, "cycles %llu\n", core->cycles_count);
	fprintf(core->files->stats, "instructions %llu\n", core->instructions_exe_count);
	fprintf(core->files->stats, "read_hit %llu\n", core->cache->read_hit_count);
	fprintf(core->files->stats, "write_hit %llu\n", core->cache->write_hit_count);
	fprintf(core->files->stats, "read_miss %llu\n", core->cache->read_miss_count);
	fprintf(core->files->stats, "write_miss %llu\n", core->cache->write_miss_count);
	fprintf(core->files->stats, "decode_stall %llu\n", core->decode_stall_count);
	fprintf(core->files->stats, "mem_stall %llu\n", core->mem_stall_count);
}

void core_init(core_t *core, core_files_t *files, cache_t *cache, uint8_t core_id)
{
	core->id = core_id;
	core->files = files;
	core->cache = cache;
	core->imem = (instruction_memory_t *)malloc(sizeof(instruction_memory_t));
	core->halted = FALSE;
	for (int i = 0; i < 16; i++)
		core->registers[i] = 0;

	core->cycles_count = 0;
	core->instructions_exe_count = 0;
	core->decode_stall_count = 0;
	core->mem_stall_count = 0;

	core->fetch.stop = 0;
	core->fetch.pc = 0;
	core->fetch.instruction = 0;
	core->fetch.stalls = 0;
	core->fetch.delay_slot = 0;
	core->fetch.delay_slot_instruction = 0;
	core->decode.pc = 0;
	core->decode.instruction = 0;
	core->decode.stalls = 0;
	core->decode.opcode = 0;
	core->decode.rs = 0;
	core->decode.rt = 0;
	core->decode.rd = 0;
	core->decode.imm = 0;
	core->execute.pc = 0;
	core->execute.instruction = 0;
	core->execute.opcode = 0;
	core->execute.rtv = 0;
	core->execute.rsv = 0;
	core->execute.rd = 0;
	core->execute.rdv = 0;
	core->execute.imm = 0;
	core->execute.alu_result = 0;
	core->execute.do_work = 0;
	core->memory_access.pc = 0;
	core->memory_access.instruction = 0;
	core->memory_access.opcode = 0;
	core->memory_access.state = MEM_ACCESS_NONE;
	core->memory_access.rtv = 0;
	core->memory_access.rd = 0;
	core->memory_access.rdv = 0;
	core->memory_access.alu_result = 0;
	core->memory_access.do_work = 0;
	core->memory_access.action_success = 1;
	core->write_back.pc = 0;
	core->write_back.instruction = 0;
	core->write_back.rd = 0;
	core->write_back.mem_data = 0;
	core->write_back.do_work = 0;
	core->write_back.opcode = 0;

	instruction_memory_load(core->imem, core->files->imem);
}

void core_free(core_t *core)
{
	free(core->imem);
}

void core_instruction_fetch(core_t *core)
{
	printf("core #%d. FETCH step \n", core->id);

	if (core->halted)
		return;
	if (core->fetch.stop) {
		return;
	}
	if (core->fetch.stalls)
	{
		core->fetch.stalls--;
		return;
	}
	core->fetch.instruction = instruction_memory_read(core->imem, core->fetch.pc);
	
	if (core->fetch.delay_slot) {
		core->decode.pc = core->fetch.pc;
		core->fetch.pc = core->fetch.delay_slot_instruction;
		core->fetch.delay_slot = 0;
	}
	else{
		core->decode.pc = core->fetch.pc;
		core->fetch.pc++;
	}
	
	core->decode.instruction = core->fetch.instruction;
}

uint32_t core_is_data_hazard(core_t *core)
{
	/*
	SW does not write to any register -> no hazard
	JAL writes to register 15 only, so only if this is rt or rs there is a hazard
	BEQ, BNE, BLT, BGT, BLE, BGE do not write to any register -> no hazard
	*/
	printf("in core_is_data_hazard op = %02x,rs=%d, rd = %d \n", core->decode.opcode, core->decode.rs, core->write_back.rd);
	if (!(core->execute.opcode == OPCODE_SW || core->execute.opcode == OPCODE_LW || (core->execute.opcode >= 9 && core->execute.opcode <= 14)))
		if (!(core->decode.opcode == OPCODE_LW) && !(core->decode.opcode == OPCODE_SW) && (core->decode.rt == core->execute.rd || core->decode.rs == core->execute.rd)) {
			return 2;
		}
	if (!(core->decode.opcode == OPCODE_SW || core->decode.opcode == OPCODE_LW || (core->decode.opcode >= 9 && core->decode.opcode <= 14)))
		if (core->memory_access.opcode == OPCODE_LW) {
			if(core->decode.rd == core->memory_access.rd){
				return 2;
			}
		}

	if (!(core->decode.opcode == OPCODE_SW || core->decode.opcode == OPCODE_LW || (core->decode.opcode >= 9 && core->decode.opcode <= 14)))
		if (core->write_back.opcode == OPCODE_LW) {
			if (core->decode.rd == core->write_back.rd) {
				return 2;
			}
		}

	if (core->execute.opcode == OPCODE_JAL)
		if (core->decode.rt == 15 || core->decode.rs == 15) {
			return 2;
		}

	if (!(core->memory_access.opcode == OPCODE_SW || core->execute.opcode == OPCODE_LW || (core->memory_access.opcode >= 9 && core->memory_access.opcode <= 14)))
		if (!(core->decode.opcode == OPCODE_LW) && !(core->decode.opcode == OPCODE_SW) && (core->decode.rt == core->memory_access.rd || core->decode.rs == core->memory_access.rd)) {
			return 1;
		}
	if (core->memory_access.opcode == OPCODE_JAL)
		if (core->decode.rt == 15 || core->decode.rs == 15) {
			return 1;
		}
	return 0;
}

bool_t check_hazard(core_t* core) {
	if (core->decode.opcode >= 9 && core->decode.opcode <= 14)
		if (core->decode.rs == core->write_back.rd || core->decode.rt == core->write_back.rd) {
			return 1;
		}
//	if (core->decode.opcode >= 9 && core->decode.opcode <= 14)
//		if (core->decode.rs == core->execute.rd || core->decode.rt == core->execute.rd)
//			return 2;
	return 0;
}

void core_instruction_decode(core_t *core)
{
	
	if (core->halted) {
		return;
	}
	if (core->decode.stalls)
	{
		core->decode.stalls--;
		return;
	}
	printf("core #%d. DECODE step instruction %08x\n", core->id, core->decode.instruction);
	// decode instruction
	core->fetch.stop = 0;
	core->decode.imm = INSTRUCTION_PARSE_BITS(core->decode.instruction, 0, INSTRUCTION_12BIT_MASK);
	core->decode.rt = INSTRUCTION_PARSE_BITS(core->decode.instruction, 12, INSTRUCTION_4BIT_MASK);
	core->decode.rs = INSTRUCTION_PARSE_BITS(core->decode.instruction, 16, INSTRUCTION_4BIT_MASK);
	core->decode.rd = INSTRUCTION_PARSE_BITS(core->decode.instruction, 20, INSTRUCTION_4BIT_MASK);
	core->decode.opcode = INSTRUCTION_PARSE_BITS(core->decode.instruction, 24, INSTRUCTION_8BIT_MASK);
	// stall if Read after Write
	uint32_t num_stalls = core_is_data_hazard(core);
	if (core->decode.instruction != 0) {
		if (num_stalls)
		{
			//core->fetch.stalls = max(num_stalls, core->fetch.stalls);
			core->decode.stalls = max(num_stalls, core->decode.stalls);
			core->fetch.stop = 1;
			core->execute.do_work = 1;
			core->memory_access.do_work = 1;
			core->write_back.do_work = 1;

			core->execute.pc = 0;
			core->execute.instruction = 0;
			core->execute.opcode = 0;
			core->execute.rtv = 0;
			core->execute.rsv = 0;
			core->execute.rd = 0;
			core->execute.rdv = 0;
			core->execute.imm = 0;

			return;
		}
	}
	if (check_hazard(core)) {
		core->fetch.stop = 1;
		return;
	}

	// write to register 1 the imm value after sign extension
	register_write(core->registers, 1, (uint32_t)core->decode.imm);

	uint32_t rtv = register_read(core->registers, core->decode.rt);
	uint32_t rsv = register_read(core->registers, core->decode.rs);
	uint32_t rdv = register_read(core->registers, core->decode.rd);

	// pass data to execute stage
	core->execute.pc = core->decode.pc;
	core->execute.instruction = core->decode.instruction;
	core->execute.opcode = core->decode.opcode;
	core->execute.rtv = rtv;
	core->execute.rsv = rsv;
	core->execute.rd = core->decode.rd;
	core->execute.rdv = rdv;
	core->execute.imm = (uint32_t)core->decode.imm;
	core->execute.do_work = 1;

	if (core->decode.opcode < 9 || core->decode.opcode > 15)
		return;
	switch (core->decode.opcode)
		{
		case OPCODE_BEQ:
			if (rtv == rsv) {
				core->fetch.delay_slot = 1;
				core->fetch.delay_slot_instruction = rdv & INSTRUCTION_10BIT_MASK;
				//core->fetch.pc = rdv & INSTRUCTION_10BIT_MASK;
			}
			break;
		case OPCODE_BNE:
			if (rtv != rsv) {
				core->fetch.delay_slot = 1;
				core->fetch.delay_slot_instruction = rdv & INSTRUCTION_10BIT_MASK;
				//core->fetch.pc = rdv & INSTRUCTION_10BIT_MASK;
			}
			break;
		case OPCODE_BLT:
			if (rsv < rtv) {
				core->fetch.delay_slot = 1;
				core->fetch.delay_slot_instruction = rdv & INSTRUCTION_10BIT_MASK;
				//core->fetch.pc = rdv & INSTRUCTION_10BIT_MASK;
			}
			break;
		case OPCODE_BGT:
			if (rsv > rtv) {
				core->fetch.delay_slot = 1;
				core->fetch.delay_slot_instruction = rdv & INSTRUCTION_10BIT_MASK;
				//core->fetch.pc = rdv & INSTRUCTION_10BIT_MASK;
			}
			break;
		case OPCODE_BLE:
			if (rsv <= rtv) {
				core->fetch.delay_slot = 1;
				core->fetch.delay_slot_instruction = rdv & INSTRUCTION_10BIT_MASK;
				//core->fetch.pc = rdv & INSTRUCTION_10BIT_MASK;
			}
			break;
		case OPCODE_BGE:
			if (rsv >= rtv) {
				core->fetch.delay_slot = 1;
				core->fetch.delay_slot_instruction = rdv & INSTRUCTION_10BIT_MASK;
				//core->fetch.pc = rdv & INSTRUCTION_10BIT_MASK;
			}
			break;
		case OPCODE_JAL:
			core->fetch.delay_slot = 1;
			core->fetch.delay_slot_instruction = rdv & INSTRUCTION_10BIT_MASK;
			//core->fetch.pc = rdv & INSTRUCTION_10BIT_MASK;
			break;
		}
	
}

void core_execute(core_t *core)
{
	if (core->halted)
		return;
	if (!core->execute.do_work) {
		return;
	}

	printf("core #%d. EX instruction %08x\n", core->id, core->execute.instruction);

	// execute instruction
	switch (core->execute.opcode)
	{
	case OPCODE_ADD:
		core->execute.alu_result = core->execute.rsv + core->execute.rtv;
		break;
	case OPCODE_SUB:
		core->execute.alu_result = core->execute.rsv - core->execute.rtv;
		break;
	case OPCODE_AND:
		core->execute.alu_result = core->execute.rsv & core->execute.rtv;
		break;
	case OPCODE_OR:
		core->execute.alu_result = core->execute.rsv | core->execute.rtv;
		break;
	case OPCODE_XOR:
		core->execute.alu_result = core->execute.rsv ^ core->execute.rtv;
		break;
	case OPCODE_MUL:
		core->execute.alu_result = core->execute.rsv * core->execute.rtv;
		break;
	case OPCODE_SLL:
		core->execute.alu_result = core->execute.rsv << core->execute.rtv;
		break;
	case OPCODE_SRA:
		core->execute.alu_result = core->execute.rsv >> core->execute.rtv;
		break;
	case OPCODE_SRL:
		core->execute.alu_result = (uint32_t)core->execute.rsv >> core->execute.rtv;
		break;
	case OPCODE_BEQ:
	case OPCODE_BNE:
	case OPCODE_BLT:
	case OPCODE_BGT:
	case OPCODE_BLE:
	case OPCODE_BGE:
		core->memory_access.rd = 0;
		break;
	case OPCODE_JAL:
		core->memory_access.rd = 15;
		core->execute.alu_result = core->decode.pc; // TODO: check if this is correct, should be the next instruction
		break;
	case OPCODE_LW:
		core->execute.alu_result = core->execute.rsv + core->execute.rtv;
		core->memory_access.state = MEM_ACCESS_READ;
		break;
	case OPCODE_SW:
		core->execute.alu_result = core->execute.rsv + core->execute.rtv;
		core->memory_access.state = MEM_ACCESS_WRITE;
		break;
	case OPCODE_HALT:
		core->halted = TRUE;
		//core->memory_access.empty = 1;
		//core->memory_access.do_work = 0;
		//core->write_back.do_work = 0;
		break;
	}

	if (!core->halted) {
		core->memory_access.do_work = 1;
	}
	core->memory_access.pc = core->execute.pc;
	core->memory_access.opcode = core->execute.opcode;
	core->memory_access.instruction = core->execute.instruction;
	core->memory_access.rtv = core->execute.rtv;
	core->memory_access.rd = core->execute.rd;
	core->memory_access.rdv = core->execute.rdv;
	//core->memory_access.state = MEM_ACCESS_NONE;
	//if (!core->halted) {
		//core->memory_access.do_work = 1;
	//}
	core->memory_access.do_work = 1;
	core->memory_access.alu_result = core->execute.alu_result;
}

void core_memory_access(core_t *core)
{
	if (!core->memory_access.do_work) {
		return;
	}
	printf("core #%d. MEMORY step instruction %08x\n", core->id, core->memory_access.instruction);
	// memory access
	core->write_back.pc = core->memory_access.pc;
	core->write_back.instruction = core->memory_access.instruction;
	core->write_back.rd = core->memory_access.rd;
	core->write_back.mem_data = core->memory_access.alu_result; // write back the alu result
	core->write_back.do_work = 1;

	if (core->memory_access.state == MEM_ACCESS_NONE) {
		return;
	}

	if (core->memory_access.state == MEM_ACCESS_READ)
	{
		// if the memory access is a read, read the data from memory and replace the mem_data value
		core->memory_access.action_success = cache_read(core->cache, core->memory_access.alu_result, &core->write_back.mem_data);
	}
	else if (core->memory_access.state == MEM_ACCESS_WRITE)
	{
		core->memory_access.action_success = cache_write(core->cache, core->memory_access.alu_result, core->memory_access.rdv);
	}


	if (!core->memory_access.action_success)
	{
		// stall the pipeline
		//core->fetch.stalls = max(1, core->fetch.stalls);
		core->fetch.stop = 1;
		core->decode.stalls = max(1, core->decode.stalls);
		core->execute.do_work = 0;
		core->memory_access.do_work = 1;
		core->write_back.do_work = 0;
		core->write_back.opcode = core->memory_access.opcode;
	}
	if (core->memory_access.action_success)
	{
		core->memory_access.state = MEM_ACCESS_NONE;
	}
}

void core_write_back(core_t *core)
{

	if (!core->write_back.do_work) {
		return;
	}
	printf("core #%d. WB step instruction %08x\n", core->id, core->write_back.instruction);

	// write back
	register_write(core->registers, core->write_back.rd, core->write_back.mem_data); // mem data can be either ALU result or memory data
}

void core_clk(core_t *core)
{
	if (core->halted)
		return;
	core->cycles_count++;
	core_write_back(core);
	core_memory_access(core);
	core_execute(core);
	core_instruction_decode(core);
	core_instruction_fetch(core);
}