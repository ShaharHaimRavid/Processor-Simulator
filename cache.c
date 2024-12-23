#include "cache.h"

uint32_t cache_read(cache_t* c, uint32_t addr) { return 0; }
void cache_write(cache_t* c, uint32_t addr, uint32_t data){}
MESI cache_get_MESI(cache_t* c, uint32_t addr, uint16_t* tag_out) {
	return MESI_INVALID;
}