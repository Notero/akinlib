# 05 — Sparse-Set ECS

Build a small entity system with inheritance + virtual functions first, then rebuild the identical system as a data-oriented sparse-set ECS (no virtual dispatch), and benchmark both on the same workload.

## Prerequisites

Finish 04 (CMake + Sanitized Bug Hunt) — you need the CMake/sanitizer setup in place to build and profile two variants of this project cleanly. Familiarity with 02's container work helps since the sparse set is itself a custom container.

## Learning objectives

- Virtual functions, vtables, dynamic dispatch
- Polymorphism via inheritance (the C++ way — you've done this manually with function-pointer tables in C)
- Data-oriented design: structure-of-arrays vs array-of-structs
- Cache locality and why it matters at runtime, not just in theory

## Why this project

You've built vtables by hand in C (struct of function pointers). This project shows you what the compiler does for you with `virtual` — and then shows you why engines increasingly avoid it. Building both versions of the same system and benchmarking them turns "cache locality matters" from a slogan into a number you measured yourself. This ECS becomes the simulation core for the capstone (06).

## Core concepts to research first

- `virtual` functions, vtable layout, vtable pointer overhead per object
- abstract base classes, pure virtual functions, virtual destructors (why a base class needs one)
- dynamic dispatch cost (indirect call, branch prediction impact)
- sparse set data structure (dense array + sparse lookup array, O(1) insert/remove/lookup)
- structure-of-arrays (SoA) vs array-of-structs (AoS)
- cache lines, spatial locality, false sharing (revisit from 03)
- `std::variant` / `std::visit` as a middle ground worth knowing about (not required, but relevant context)

## Build milestones

1. **Version A (OOP):** Define an abstract `Entity` base class with `virtual void update()`, derive a few concrete entity types, store them as `vector<unique_ptr<Entity>>`, and update all of them each tick.
2. Benchmark Version A: N entities, M ticks, measure wall-clock time and note cache-miss behavior (if you have `perf`/Instruments available).
3. **Version B (ECS):** Design components as plain data structs (no virtuals), and a sparse set per component type mapping entity ID → dense component index.
4. Implement entity creation/destruction and component add/remove against the sparse set, keeping the dense arrays contiguous (swap-and-pop on removal).
5. Implement the same per-tick update logic as Version A but iterating dense component arrays directly (no dispatch, no pointer chasing).
6. Run the identical workload (same N, same M) through Version B and produce a benchmark comparison against Version A with real numbers.

## Self-check questions

- Draw (on paper or out loud) what a vtable lookup actually does at the machine level when you call a virtual function through a base pointer.
- Why does a sparse set give you O(1) insert/remove/lookup while keeping the dense array contiguous — what's the trick with the sparse-array indirection?
- Explain your benchmark results in terms of cache lines: why was Version B faster (or not) for your specific workload and entity/component sizes?
- When would you actually still prefer the OOP/virtual approach over the ECS approach in a real project?

## Suggested tools/references

- cppreference: [Virtual function](https://en.cppreference.com/w/cpp/language/virtual), [Abstract class](https://en.cppreference.com/w/cpp/language/abstract_class)
- Headers: `<vector>`, `<memory>`, `<chrono>` (for benchmarking)
- `perf stat` / `perf record` (Linux) or Instruments (macOS) for cache-miss profiling
- Background reading: any "sparse set ECS" writeup (e.g. the EnTT project's design notes) for the data structure shape — implement your own, don't just use the library
- `-O2`/`-O3` for benchmark builds (Debug-build numbers are meaningless here)
