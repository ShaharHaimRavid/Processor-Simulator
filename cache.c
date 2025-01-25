#include "cache.h"
#include "main_memory.h"

#define METADATA_TAG(x) (x & 0xFFF)
#define METADATA_MESI(x) ((x >> 12) & 0x3)
#define ADDRESS_TAG(x) ((x >> 8) & 0xFFF)
#define ADDRESS_INDEX(x) ((x >> 4) & 0xF)
#define ADDRESS_OFFSET(x) ((x & 0xF) >> 2)
#define CREATE_META_DATA(tag, mesi) (tag | (mesi << 12))

bool_t cache_find(cache_t *c, uint32_t addr, uint8_t *index_of_block)
{
	addr = addr & 0xFFFFF; // addr is 20 bits
	uint16_t tag = ADDRESS_TAG(addr);
	uint16_t index = ADDRESS_INDEX(addr);

	bool_t found = FALSE;
	for (int i = 0; i < 4; i++)
	{
		uint16_t meta_of_block = c->metadata[index + i];
		if (METADATA_TAG(meta_of_block) != tag)
			continue;

		found = TRUE;
		*index_of_block = index + i;
	}
	return found;
}

void cache_flush(cache_t *c, uint8_t index_of_block)
{
	printf("cache_flush: index_of_block=%d\n", index_of_block);
	uint16_t tag = METADATA_TAG(c->metadata[index_of_block]);
	uint16_t index = index_of_block / 4;
	uint16_t addr = (tag << 8) | (index << 4) | 0x0;

	main_memory_bus_write(c->bus, addr, c->data[index_of_block]);
}

void cache_snoop(bus_origid_t origid, bus_command_t cmd, bus_addr_t addr, uint32_t data, bool_t *shared, void *user_data)
{
	cache_t *c = (cache_t *)user_data;
	if (c->id == origid)
		return;

	uint16_t tag = ADDRESS_TAG(addr);
	uint16_t set = ADDRESS_INDEX(addr);
	uint16_t offset = ADDRESS_OFFSET(addr);

	uint8_t index_of_block;
	bool_t found = cache_find(c, addr, &index_of_block);

	printf("cache_snoop: origid=%d, cmd=%d, addr=0x%08X, data=0x%08X, found=%d\n", origid, cmd, addr, data, found);

	*shared |= found; // raise shared bit if found

	if (origid == BUS_ORIGID_MAIN_MEMORY)
	{
		if (cmd != BUS_COMMAND_FLUSH || c->pending_addr != addr)
			return;

		if (c->pending_addr % 4 == 3) // last word of block
		{
			c->pending_addr = 0;
			c->metadata[set] = CREATE_META_DATA(tag, shared ? MESI_SHARED : MESI_EXCLUSIVE);
			printf("cache_snoop: main memory flush complete, set=%d, tag=0x%03X\n", set, tag);
		}

		c->data[set][offset] = data;
		c->pending_addr++;
		printf("cache_snoop: main memory write, set=%d, offset=%d, data=0x%08X\n", set, offset, data);
		return;
	}

	if (!found) // not in cache - shouldn't change MESI state
		return;

	MESI mesi = METADATA_MESI(c->metadata[index_of_block]);
	printf("cache_snoop: MESI state=%d, index_of_block=%d\n", mesi, index_of_block);

	// ========================================
	// MESI Protocol - handle status of blocks
	// ========================================
	switch (mesi)
	{
	case MESI_MODIFIED:
		if (cmd == BUS_COMMAND_READ)
		{
			cache_flush(c, index_of_block);
			c->metadata[index_of_block] = CREATE_META_DATA(tag, MESI_SHARED);
			printf("cache_snoop: MESI_MODIFIED -> MESI_SHARED, index_of_block=%d\n", index_of_block);
		}
		else if (cmd == BUS_COMMAND_READX)
		{
			cache_flush(c, index_of_block);
			c->metadata[index_of_block] = CREATE_META_DATA(tag, MESI_INVALID);
			printf("cache_snoop: MESI_MODIFIED -> MESI_INVALID, index_of_block=%d\n", index_of_block);
		}
		break;
	case MESI_EXCLUSIVE:
		if (cmd == BUS_COMMAND_READ)
		{
			c->metadata[index_of_block] = CREATE_META_DATA(tag, MESI_SHARED);
			printf("cache_snoop: MESI_EXCLUSIVE -> MESI_SHARED, index_of_block=%d\n", index_of_block);
		}
		else if (cmd == BUS_COMMAND_READX)
		{
			c->metadata[index_of_block] = CREATE_META_DATA(tag, MESI_INVALID);
			printf("cache_snoop: MESI_EXCLUSIVE -> MESI_INVALID, index_of_block=%d\n", index_of_block);
		}
		break;
	case MESI_SHARED:
		if (cmd == BUS_COMMAND_READX)
		{
			c->metadata[index_of_block] = CREATE_META_DATA(tag, MESI_INVALID);
			printf("cache_snoop: MESI_SHARED -> MESI_INVALID, index_of_block=%d\n", index_of_block);
		}
		break;
	}
}

void cache_init(cache_t *c, main_memory_bus_t *bus, int8_t id)
{
	c->read_hit_count = 0;
	c->read_miss_count = 0;
	c->write_hit_count = 0;
	c->write_miss_count = 0;
	for (int i = 0; i < 64; i++)
	{
		c->metadata[i] = 0;
	}
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			c->data[i][j] = 0;
		}
	}
	c->bus = bus;
	c->id = id;
	c->pending_addr = 0;
	main_memory_bus_snoop_observe(bus, c->id, cache_snoop, c);
}

bool_t cache_read(cache_t *c, uint32_t addr, uint32_t *data)
{
	printf("cache_read: addr=0x%08X\n", addr);
	uint8_t index_of_block;
	bool_t found = cache_find(c, addr, &index_of_block);

	uint16_t tag = ADDRESS_TAG(addr);
	uint16_t set = ADDRESS_INDEX(addr);
	uint16_t offset = ADDRESS_OFFSET(addr);

	MESI mesi = METADATA_MESI(c->metadata[index_of_block]);

	if (found && mesi != MESI_INVALID)
	{
		// data is in cache
		*data = c->data[index_of_block][offset];
		printf("cache_read: hit, data=0x%08X\n", *data);
		return TRUE;
	}

	// no data in cache (or invalid), need to fetch from memory
	printf("cache_read: miss, fetching from memory\n");
	word w;
	bool_t shared;
	bool_t ret = main_memory_bus_action(c->bus, c->id, addr, BUS_COMMAND_READ, &w, &shared);
	if (ret) // got transaction from round-robin
		c->pending_addr = BLOCK_ADDR_FROM_BYTE(addr);
	printf("cache_read: got ret %d \n", ret);
	return FALSE;
}

bool_t cache_write(cache_t *c, uint32_t addr, uint32_t data)
{
	printf("cache_write: addr=0x%08X, data=0x%08X\n", addr, data);
	uint8_t index_of_block;
	bool_t found = cache_find(c, addr, &index_of_block);

	uint16_t tag = ADDRESS_TAG(addr);
	uint16_t set = ADDRESS_INDEX(addr);
	uint16_t offset = ADDRESS_OFFSET(addr);

	MESI mesi = METADATA_MESI(c->metadata[index_of_block]);

	if (!found || mesi == MESI_INVALID || mesi == MESI_SHARED)
	{
		printf("cache_write: miss or invalid/shared, fetching from memory\n");
		word w;
		bool_t shared;
		bool_t ret = main_memory_bus_action(c->bus, c->id, addr, BUS_COMMAND_READX, &w, &shared);
		if (!ret)
		{
			printf("cache_write: memory fetch failed\n");
			return FALSE;
		}
	}

	c->data[index_of_block][offset] = data;
	c->metadata[index_of_block] = tag | MESI_MODIFIED << 12;
	printf("cache_write: write complete\n");
	return TRUE;
}

// Save the data array of the cache to the dsram file
void dsram_save(cache_t *c, FILE *dsram)
{
	// Iterate over the 64 blocks in the cache
	for (int i = 0; i < 64; i++)
	{
		// Each block contains 4 words
		for (int j = 0; j < 4; j++)
		{
			// Print each word as 8 hexadecimal digits
			fprintf(dsram, "%08X\n", c->data[i][j]);
		}
	}
}

// Save the metadata array of the cache to the tsram file
void tsram_save(cache_t *c, FILE *tsram)
{
	// Validate inputs
	if (!c || !tsram)
	{
		return;
	}

	// Iterate over the 64 metadata entries in the cache
	for (int i = 0; i < 64; i++)
	{
		// Print each metadata entry as 8 hexadecimal digits (extending with zeros to fill 32 bits)
		fprintf(tsram, "%08X\n", c->metadata[i]);
	}
}
