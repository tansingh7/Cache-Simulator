#include <stdio.h>
#include "cachesim.h"

#define MEMSIZE 2048
#define LOWSTART 0x20000U
#define HIGHSTART 0x80000U

#define L1s 2
#define L1E 2
#define L1DELAY 7

#define L2s 3
#define L2E 5
#define L2DELAY 30

void init_mem(int memsize, addr_t low, addr_t high);

static void print_access_json(addr_t addr, unsigned value, int delay, cache_t *l1, cache_t *l2)
{
    cache_location_t l1_active;
    cache_location_t l2_active;
    cache_location_t *l1_active_ptr = NULL;
    cache_location_t *l2_active_ptr = NULL;

    if (cache_find_line(l1, addr, &l1_active)) {
        l1_active_ptr = &l1_active;
    }

    if (delay > L1DELAY && cache_find_line(l2, addr, &l2_active)) {
        l2_active_ptr = &l2_active;
    }

    printf("    {\n");
    printf("      \"address\":\"0x%llx\",\n", (unsigned long long)addr);
    printf("      \"value\":\"%02x\",\n", value & 0xffU);
    printf("      \"cycles\":%d,\n", delay);
    printf("      \"status\":\"%s\",\n", delay == L1DELAY ? "HIT" : "MISS");
    printf("      \"l1\":");
    cache_export_json(stdout, l1, l1_active_ptr);
    printf(",\n");
    printf("      \"l2\":");
    cache_export_json(stdout, l2, l2_active_ptr);
    printf("\n");
    printf("    }");
}

int main()
{
    addr_t a, end;
    int blocksize = 1 << LOGBSIZE;
    int i;
    unsigned char fooey;

    int hits = 0;
    int misses = 0;
    int totalCycles = 0;
    int first = 1;

    init_mem(MEMSIZE, LOWSTART, HIGHSTART);

    cache_t *l2 = cache_create(L2s, L2E, L2DELAY, NULL);
    cache_t *l1 = cache_create(L1s, L1E, L1DELAY, l2);

    printf("{\n");
    printf("  \"blockSize\": %d,\n", blocksize);
    printf("  \"accesses\": [\n");

    /* LOW MEMORY */

    end = LOWSTART + (MEMSIZE >> 1);

    for (a = LOWSTART; a < end; a += blocksize)
    {
        for (i = 0; i < blocksize; i++)
        {
            int delay = cache_access(l1, a + i, &fooey, 1, 0);

            if (delay == L1DELAY)
                hits++;
            else
                misses++;

            totalCycles += delay;

            if (!first)
                printf(",\n");

            first = 0;

            print_access_json(a + i, fooey, delay, l1, l2);
        }
    }

    /* HIGH MEMORY */

    end = HIGHSTART + (MEMSIZE >> 1);

    for (a = HIGHSTART; a < end; a += blocksize)
    {
        for (i = 0; i < blocksize; i++)
        {
            int delay = cache_access(l1, a + i, &fooey, 1, 0);

            if (delay == L1DELAY)
                hits++;
            else
                misses++;

            totalCycles += delay;

            printf(",\n");

            print_access_json(a + i, fooey, delay, l1, l2);
        }
    }

    printf("\n  ],\n");

    printf("  \"statistics\": {\n");
    printf("      \"hits\": %d,\n", hits);
    printf("      \"misses\": %d,\n", misses);
    printf("      \"totalCycles\": %d,\n", totalCycles);
    printf("      \"hitRate\": %.2f\n",
           (hits*100.0)/(hits+misses));
    printf("  }\n");

    printf("}\n");

    return 0;
}
