# CS 270 – Project 4: Hardware Cache Simulator

> **Course:** CS 270: Systems Programming – Spring 2025, University of Kentucky
> **Author:** Hamblet Arroyo

---

## Overview

A C implementation of a hardware cache simulator that models the behavior of a multi-level processor cache (L1, L2, etc.). Built as a standalone utility — no `main()` — designed to be compiled alongside any driver program.

Supports flexible cache configurations with any combination of sets (S), associativity (E), and blocksize (B), and can simulate a full hierarchy of caches chained together.

---

## Files

| File | Description |
|---|---|
| `cachesim.c` | Full cache simulator implementation |
| `cachesim.h` | Header file — structs and function signatures (provided, do not modify) |
| `driver0.c` | Read-only test driver — 2-level cache hierarchy |
| `driver1.c` | Write test driver — eviction and writeback verification |
| `memory.o` | Pre-compiled simulated memory implementation (provided) |

---

## Build & Test

```bash
# Driver 0 (read test)
gcc -Wall -o test0 driver0.c cachesim.c memory.o
./test0

# Driver 1 (write/eviction test)
gcc -Wall -o test1 driver1.c cachesim.c memory.o
./test1
```

To change the blocksize at compile time:

```bash
gcc -DLOGBSIZE=5 -Wall -o test0 driver0.c cachesim.c memory.o
```

---

## API

| Function | Description |
|---|---|
| `cache_create(s, E, delay, nextlevel)` | Allocates and returns a new cache with `2^s` sets, `E` lines per set, and a pointer to the next cache level (or `NULL` for memory) |
| `cache_access(c, addr, value, size, iswrite)` | Reads or writes `size` bytes at `addr`. On a miss, fetches from the next level. Returns total delay |
| `cache_flush(c)` | Invalidates all lines; writes back dirty lines to the next level or memory |

---

## Cache Parameters

Given an address, bits are interpreted as:

```
[ tag bits | set index bits (s) | block offset bits (LOGBSIZE) ]
```

- **LOGBSIZE** — compile-time constant; default `4` (blocksize = 16 bytes)
- **s** — log₂ of the number of sets per cache instance
- **E** — number of lines per set (associativity)

---

## Design & Policies

**Replacement policy — LRU (Least Recently Used)**

Each set maintains a `lru_order` array (stored in `useinfo`) that tracks access recency. On every hit or miss, the accessed line's index moves to the end of the array. The victim on a miss is always `lru_order[0]` — the least recently used line.

**Write policy — Writeback with dirty bit**

- Writes update the cache line and set `dirty = 1`; memory is not touched immediately
- When a dirty line is evicted (conflict miss or flush), the block is written back to the next level or directly to memory
- This keeps writes fast on a hit and defers the write penalty to eviction time

**Miss handling**

On a miss, the victim line is selected via LRU. If dirty, it is written back before the new block is fetched from the next level down. The delay returned is the sum of all levels accessed.

---

## Example Delay Behavior

With L1 delay = 7, L2 delay = 30, memory delay = 70:

| Access type | Delay |
|---|---|
| L1 hit | 7 |
| L1 miss, L2 hit | 7 + 30 = 37 |
| L1 miss, L2 miss | 7 + 30 + 70 = 107 |

First byte of each new block costs the full miss penalty; remaining bytes in the same block cost only the L1 hit delay (7).
