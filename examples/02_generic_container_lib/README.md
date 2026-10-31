# 02 — Generic Container Library

Build your own templated, STL-compatible fixed-capacity ring buffer.

## Prerequisites

Finish 01 (TCP Chat Server) — you should already be comfortable with RAII and move semantics before adding templates on top.

## Learning objectives

- Templates as compile-time generics (not C void-pointer generics)
- Writing STL-compatible iterators (`begin()`/`end()`, works with range-`for`)
- STL container complexity guarantees and why they're shaped the way they are
- Template argument deduction, `typename`, member type aliases (`value_type`, etc.)

## Why this project

You already know generics via `void*` + function pointers from C. Templates are stricter and stranger: they're compile-time, type-checked, and instantiated per type. Building one small STL-shaped container yourself is the fastest way to understand *why* `std::vector` and `std::unordered_map` are built the way they are — capacity growth, iterator invalidation rules, bucket/load-factor tradeoffs. The container you build here becomes reusable plumbing for the thread pool (03) and capstone (06).

## Core concepts to research first

- class templates vs function templates
- template parameter deduction, explicit instantiation
- iterator categories (forward, bidirectional, random-access) — you only need forward/random-access here
- what `begin()`/`end()` need to return to work with range-`for`
- `operator++`, `operator*`, `operator!=` overloads for a custom iterator
- `std::vector` growth strategy (amortized O(1) push_back)
- `std::unordered_map` bucket/hash/load-factor model
- iterator invalidation rules for `vector` vs `unordered_map`
- `noexcept` and why move operations should be marked `noexcept`
- `std::optional` as a "value or nothing" return type vs sentinel values/exceptions

## Build milestones

1. ~~Pick one: fixed-capacity ring buffer, or a simple open-addressing hash map.~~ **Chose: fixed-capacity ring buffer.** Implemented as a class template (`template <typename T>`).
2. Implement `push`/`pop` with correct semantics on full/empty cases.
3. Write a custom iterator type (nested class) supporting `operator++`, `operator*`, `operator!=`, and expose `begin()`/`end()` so range-`for` works.
4. Write a small test program that uses your container with range-`for` and with `std::algorithm` functions (e.g. `std::find`, `std::count_if`).
5. Make `pop`/`peek` return `std::optional<T>` instead of a sentinel value or exception, and justify the choice.

**Dropped (user call, 2026-10-19):** reimplementing the workload with `std::queue` and writing a comparison note — judged not valuable for this project.

## Self-check questions

- What exactly does the compiler generate when you instantiate your template for two different types — walk through it.
- What's the minimum interface an iterator type needs to implement to work with range-`for`, and why does that requirement exist (what is `for` actually calling)?
- For your container, what operation invalidates existing iterators, and does that match or differ from `std::vector`'s rule?
- Why is `noexcept` on move operations important enough that `std::vector` checks for it at compile time (hint: strong exception guarantee during reallocation)?

## Suggested tools/references

- cppreference: [Templates](https://en.cppreference.com/w/cpp/language/templates), [Iterator](https://en.cppreference.com/w/cpp/iterator), [std::vector](https://en.cppreference.com/w/cpp/container/vector), [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map)
- Headers: `<iterator>`, `<vector>`, `<unordered_map>`, `<algorithm>`, `<type_traits>`
- `static_assert` for compile-time sanity checks on your template
- Compile with `-Wall -Wextra -std=c++20`
