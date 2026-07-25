#include "cachesim.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MEM (1 << 20)   // 1 MB simulated memory

static uint8_t memory[MAX_MEM];
static addr_t low_start = 0;
static addr_t high_start = 0;
static int half_size = 0;

void init_mem(int memsize, addr_t low, addr_t high)
{
    low_start = low;
    high_start = high;
    half_size = memsize / 2;

    /* Fill the low segment */
    for (int i = 0; i < half_size; i++)
        memory[(low_start + i) % MAX_MEM] = (uint8_t)i;

    /* Fill the high segment */
    for (int i = 0; i < half_size; i++)
        memory[(high_start + i) % MAX_MEM] = (uint8_t)i;
}

int mem_access(addr_t addr, void *value, int size, int iswrite)
{
    if (addr + size >= MAX_MEM) {
        fprintf(stderr, "Memory access out of bounds: 0x%llx\n",
                (unsigned long long)addr);
        exit(EXIT_FAILURE);
    }

    if (iswrite)
        memcpy(&memory[addr], value, size);
    else
        memcpy(value, &memory[addr], size);

    /* Memory latency */
    return 70;
}