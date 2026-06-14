# CPU Architecture Simulator (4-Core Multi-Processor System)

## Overview

This project is a C-based simulator of a 4-core CPU architecture, developed as part of a Computer Architecture course. The simulator models a complete multi-core system with a realistic memory hierarchy and execution pipeline.

The system includes 4 independent processing cores running in parallel, each executing its own instruction stream using a custom assembly-like language.

Each core features a 5-stage pipeline, a private cache, and interacts with a shared main memory through a bus arbitration system. Cache coherence between cores is maintained using the MESI protocol.

---

## Architecture

The simulator models the following components:

- 4 processing cores operating concurrently
- 5-stage instruction pipeline per core (Fetch, Decode, Execute, Memory, Write Back)
- Private data cache per core
- Shared main memory
- Bus arbitration mechanism for memory access coordination
- MESI cache coherence protocol for maintaining consistency between caches

Each core runs independently but is synchronized at the cycle level to simulate realistic multi-core behavior.

---

## Input

The simulator receives as input:

- Instruction memory files for each core (`imem0.txt` - `imem3.txt`)
- Initial main memory state (`memin.txt`)

Each core executes its own instruction file independently using a custom assembly-like instruction set.

---

## Output

The simulator generates detailed output files describing the full system state after execution.

### Per-Core Outputs

Each core produces:

- Final register state (`regout`)
- Execution trace (`coretrace`)
- Cache memory dumps (`dsram`, `tsram`)
- Performance statistics (`stats`)

### Shared System Outputs

The system also produces:

- Final main memory state (`memout`)
- Bus activity trace (`bustrace`)

---

## Performance Statistics

Each core reports detailed runtime statistics used to analyze performance and bottlenecks

These metrics are used to evaluate cache efficiency, pipeline stalls, and memory access delays.

---

## Build Instructions

To compile the project using GCC:

gcc *.c -o simulator

---

## Run Instructions

### Default Mode

Runs the simulator using predefined file paths inside the code:

./simulator

### Custom Input Mode

You can provide input and output files manually:

./simulator imem0.txt imem1.txt imem2.txt imem3.txt memin.txt memout.txt regout0.txt regout1.txt regout2.txt regout3.txt core0trace.txt core1trace.txt core2trace.txt core3trace.txt bustrace.txt dsram0.txt dsram1.txt dsram2.txt dsram3.txt tsram0.txt tsram1.txt tsram2.txt tsram3.txt stats0.txt stats1.txt stats2.txt stats3.txt

---
