# CPU Architecture Simulator (Multi-Core)

## Overview

This project is a C-based simulation of a **4-core processor architecture**, designed as part of a computer architecture course.

The simulator models a simplified CPU system including:
- 4 independent processing cores running in parallel
- 5-stage instruction pipeline
- Private cache per core
- Cache coherence using the MESI protocol
- Shared main memory with arbitration mechanism

Each core executes a custom assembly-like instruction set and interacts with memory through a simulated cache hierarchy.

---

## Architecture Features

### Multi-Core System
- 4 cores operating concurrently
- Each core has its own register file and instruction stream
- Core execution is synchronized cycle-by-cycle

### Pipeline (5 Stages)
Each instruction goes through:
- Fetch
- Decode
- Execute
- Memory Access
- Write Back

### Cache System
- Each core has a private data cache (D-cache)
- Cache simulation includes:
  - Read/Write hits and misses
  - Cache line management

### Memory Consistency
- MESI coherence protocol ensures consistency between caches
- Bus arbitration controls memory access conflicts between cores

---

## Input

The simulator takes as input:

- Assembly-like instruction files for each core
- Initial memory state file

Example instruction format:
```asm
add $r2, $r2, $imm, 1
blt $zero, $r2, $imm, 100
halt $zero, $zero, $zero, 0
