#ifndef CACHESIM_H
#define CACHESIM_H

#include <stdio.h>
#include <stdint.h>

#ifndef LOGBSIZE
/* Cache blocks are 16 bytes */
#define LOGBSIZE 4
#endif

typedef uint64_t addr_t;

/* Cache Line */
typedef struct {
    addr_t tag;
    uint8_t *block;
    int valid;
    int dirty;
} cacheline_t;

/* Cache Set */
typedef struct {
    cacheline_t *lines;
    void *useinfo;
} cacheset_t;

/* Cache */
typedef struct hwcache {
    cacheset_t *cache_sets;
    struct hwcache *cache_next;

    int cache_E;      /* Associativity */
    int cache_s;      /* log2(number of sets) */
    int cache_delay;  /* Cache latency */

} cache_t;

typedef struct {
    int set;
    int way;
} cache_location_t;


/* Existing functions */

cache_t *cache_create(
    int s,
    int E,
    int delay,
    cache_t *nextlevel
);

int cache_access(
    cache_t *c,
    addr_t addr,
    void *value,
    int size,
    int iswrite
);

void cache_flush(cache_t *c);

int cache_num_sets(const cache_t *cache);

int cache_num_ways(const cache_t *cache);

int cache_find_line(const cache_t *cache, addr_t addr, cache_location_t *location);

void cache_export_json(FILE *out, const cache_t *cache, const cache_location_t *active);


/* Memory interface */

extern int mem_access(
    addr_t addr,
    void *value,
    int size,
    int iswrite
);


#endif
