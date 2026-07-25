//Hamblet Arroyo

#include "cachesim.h"
#include <stdlib.h>
#include <string.h>

#define BLOCKSIZE (1 << LOGBSIZE)  // Block size in bytes (e.g., 16 if LOGBSIZE = 4)

// Structure for LRU tracking: holds an array of indices to track usage order
typedef struct {
    int *lru_order;
} lru_info_t;

// Constructs and initializes a new cache instance
cache_t *cache_create(int s, int E, int delay, cache_t *nextlevel) {
    cache_t *cache = malloc(sizeof(cache_t));
    cache->cache_E = E;              // Lines per set
    cache->cache_s = s;              // log2(Number of sets)
    cache->cache_delay = delay;      // Delay for this level
    cache->cache_next = nextlevel;   // Pointer to next level cache or NULL

    int num_sets = 1 << s;           // Total number of sets
    cache->cache_sets = malloc(sizeof(cacheset_t) * num_sets);

    // Allocate and initialize each set
    for (int i = 0; i < num_sets; i++) {
        cacheset_t *set = &cache->cache_sets[i];
        set->lines = malloc(sizeof(cacheline_t) * E);

        // Allocate and initialize each line in the set
        for (int j = 0; j < E; j++) {
            set->lines[j].tag = 0;
            set->lines[j].valid = 0;
            set->lines[j].dirty = 0;
            set->lines[j].block = malloc(BLOCKSIZE);
        }

        // Allocate LRU info for this set
        lru_info_t *info = malloc(sizeof(lru_info_t));
        info->lru_order = malloc(sizeof(int) * E);
        for (int j = 0; j < E; j++) info->lru_order[j] = j;
        set->useinfo = info;
    }

    return cache;
}

// Updates the LRU order after a line is accessed
static void update_lru(cacheset_t *set, int accessed_index, int E) {
    lru_info_t *info = (lru_info_t *)set->useinfo;
    int *order = info->lru_order;

    // Find position of accessed line
    int pos = 0;
    while (pos < E && order[pos] != accessed_index) pos++;

    // Move accessed line to the end (most recently used)
    for (int i = pos; i < E - 1; i++) order[i] = order[i + 1];
    order[E - 1] = accessed_index;
}

// Returns the index of the least recently used (LRU) line
static int find_victim_index(cacheset_t *set) {
    lru_info_t *info = (lru_info_t *)set->useinfo;
    return info->lru_order[0];  // Oldest access at front
}

// Main function to access the cache (read or write)
int cache_access(cache_t *c, addr_t addr, void *value, int size, int iswrite) {
    // Extract block offset, set index, and tag from address
    int block_offset = addr & ((1 << LOGBSIZE) - 1);
    int set_index = (addr >> LOGBSIZE) & ((1 << c->cache_s) - 1);
    addr_t tag = addr >> (LOGBSIZE + c->cache_s);

    cacheset_t *set = &c->cache_sets[set_index];
    int E = c->cache_E;
    int delay = c->cache_delay;

    // Search for a matching tag (cache hit)
    for (int i = 0; i < E; i++) {
        cacheline_t *line = &set->lines[i];
        if (line->valid && line->tag == tag) {
            // Hit: read or write the data
            if (iswrite) {
                memcpy(&line->block[block_offset], value, size);
                line->dirty = 1;
            } else {
                memcpy(value, &line->block[block_offset], size);
            }
            update_lru(set, i, E);
            return delay;  // Only this cache level accessed
        }
    }

    // Miss: need to replace a line
    int victim = find_victim_index(set);
    cacheline_t *vline = &set->lines[victim];

    // Reconstruct full address for the victim block
    addr_t victim_addr = ((vline->tag << c->cache_s) | set_index) << LOGBSIZE;

    // If victim is dirty, write back to lower level
    if (vline->valid && vline->dirty) {
        if (c->cache_next) {
            cache_access(c->cache_next, victim_addr, vline->block, BLOCKSIZE, 1);
        } else {
            mem_access(victim_addr, vline->block, BLOCKSIZE, 1);
        }
    }

    // Load new block from lower level
    if (c->cache_next) {
        delay += cache_access(c->cache_next, addr - block_offset, vline->block, BLOCKSIZE, 0);
    } else {
        delay += mem_access(addr - block_offset, vline->block, BLOCKSIZE, 0);
    }

    // Update victim line with new data
    vline->tag = tag;
    vline->valid = 1;
    vline->dirty = 0;

    // Perform read/write operation after loading
    if (iswrite) {
        memcpy(&vline->block[block_offset], value, size);
        vline->dirty = 1;
    } else {
        memcpy(value, &vline->block[block_offset], size);
    }

    update_lru(set, victim, E);
    return delay;
}

// Invalidates all lines and writes dirty ones back to lower memory/cache
void cache_flush(cache_t *c) {
    int num_sets = 1 << c->cache_s;

    for (int i = 0; i < num_sets; i++) {
        cacheset_t *set = &c->cache_sets[i];

        for (int j = 0; j < c->cache_E; j++) {
            cacheline_t *line = &set->lines[j];

            // Write back dirty line to lower level
            if (line->valid && line->dirty) {
                addr_t addr = ((line->tag << c->cache_s) | i) << LOGBSIZE;
                if (c->cache_next) {
                    cache_access(c->cache_next, addr, line->block, BLOCKSIZE, 1);
                } else {
                    mem_access(addr, line->block, BLOCKSIZE, 1);
                }
            }

            // Invalidate line
            line->valid = 0;
            line->dirty = 0;
        }
    }
}

int cache_num_sets(const cache_t *cache) {
    if (!cache) {
        return 0;
    }

    return 1 << cache->cache_s;
}

int cache_num_ways(const cache_t *cache) {
    if (!cache) {
        return 0;
    }

    return cache->cache_E;
}

int cache_find_line(const cache_t *cache, addr_t addr, cache_location_t *location) {
    if (!cache || !location) {
        return 0;
    }

    int set_index = (addr >> LOGBSIZE) & ((1 << cache->cache_s) - 1);
    addr_t tag = addr >> (LOGBSIZE + cache->cache_s);
    const cacheset_t *set = &cache->cache_sets[set_index];

    for (int way = 0; way < cache->cache_E; way++) {
        const cacheline_t *line = &set->lines[way];

        if (line->valid && line->tag == tag) {
            location->set = set_index;
            location->way = way;
            return 1;
        }
    }

    return 0;
}

void cache_export_json(FILE *out, const cache_t *cache, const cache_location_t *active) {
    int sets = cache_num_sets(cache);
    int ways = cache_num_ways(cache);

    fprintf(out, "{\n");
    fprintf(out, "        \"sets\":%d,\n", sets);
    fprintf(out, "        \"ways\":%d,\n", ways);

    if (active) {
        fprintf(out, "        \"active\":{\"set\":%d,\"way\":%d},\n", active->set, active->way);
    }

    fprintf(out, "        \"cache\":[\n");

    for (int set_index = 0; set_index < sets; set_index++) {
        fprintf(out, "          [\n");

        for (int way = 0; way < ways; way++) {
            const cacheline_t *line = &cache->cache_sets[set_index].lines[way];

            fprintf(out, "            {\"tag\":\"0x%llx\",\"valid\":%s,\"dirty\":%s}",
                    (unsigned long long)line->tag,
                    line->valid ? "true" : "false",
                    line->dirty ? "true" : "false");

            if (way < ways - 1) {
                fprintf(out, ",");
            }

            fprintf(out, "\n");
        }

        fprintf(out, "          ]");

        if (set_index < sets - 1) {
            fprintf(out, ",");
        }

        fprintf(out, "\n");
    }

    fprintf(out, "        ]\n");
    fprintf(out, "      }");
}
