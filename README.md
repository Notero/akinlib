# library

Shared C++ components built across the [CPP/Path](https://github.com/Notero) project series: RAII wrappers, concurrency primitives, and data structures, each written from scratch as a learning exercise.

## Components

| File | What it is |
|---|---|
| `Connection.hpp/cpp` | RAII wrapper around a TCP socket |
| `Database.hpp/cpp` | SQLite-backed persistence layer |
| `Client.hpp/cpp` | TCP client wrapper |
| `RingBuffer.hpp` | Fixed-capacity templated ring buffer with STL-style iterators |
| `ORingBuffer.hpp` | Ring buffer variant used for network snapshot/input buffering |
| `SafeQueue.hpp` | Thread-safe job queue (mutex + condition_variable) |
| `SparseSet.hpp` | Sparse-set ECS component storage (O(1) insert/remove/lookup) |
| `EventHandler.hpp` | Variadic-template event dispatcher |
| `Simulation.hpp` | Tick-based simulation state |
| `Structs.hpp` | Shared plain-data types (wire format structs) |
| `UIDGenerator.hpp` | Entity/connection ID generation |
| `Pub.hpp` | Publish/broadcast helper |

## Examples

`examples/` holds standalone driver/benchmark programs that exercise these components in isolation, each with its own README: generic-container usage, thread-pool benchmarking, OOP-vs-ECS entity benchmarking, and compile-time-generics experiments (CRTP, variadic templates, type lists).

## Build

```
cmake -S . -B build
cmake --build build
```
