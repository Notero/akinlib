# 07 — Compile-Time C++ & Generic Programming Mastery

Push templates past "generic container" into compile-time programming: `constexpr`, metaprogramming, C++20 concepts, CRTP.

## Prerequisites

Finish 02 (Generic Container Library) and 05 (Sparse-Set ECS). You need a template you've already written (02) and a virtual-dispatch system you've already benchmarked (05) to have something concrete to rework here.

## Learning objectives

- `constexpr`/`consteval` functions and compile-time evaluation
- Template metaprogramming: SFINAE, `std::enable_if`, type traits
- C++20 concepts and `requires` clauses
- Variadic templates, parameter packs, fold expressions
- CRTP (Curiously Recurring Template Pattern) as static polymorphism
- `if constexpr` for compile-time branching

## Why this project

Project 02 taught you templates as "generics that get stamped out per type." That's necessary but not sufficient — most modern (post-2020) C++ codebases lean on concepts, `constexpr`, and CRTP to express constraints and static polymorphism that used to require SFINAE hacks or virtual dispatch. This is also exactly the territory technical interviews probe when checking whether someone actually knows C++ vs. just writes working C++. It directly revisits 05's virtual-vs-static-dispatch question with a third option (CRTP) now on the table.

## Core concepts to research first

- `constexpr` functions vs `consteval` (C++20) — when evaluation is guaranteed at compile time vs merely allowed
- `if constexpr` vs runtime `if` — why it lets templates compile for branches that wouldn't otherwise type-check
- SFINAE, `std::enable_if`, `<type_traits>` basics (`is_same`, `is_move_constructible`, etc.)
- C++20 concepts: `concept`, `requires` clauses, constrained templates
- variadic templates, parameter packs, fold expressions (C++17)
- CRTP: static polymorphism via `class Derived : Base<Derived>`
- template specialization: full vs partial
- compile-time cost: what heavy template metaprogramming does to build times and error messages

## Build milestones

1. Take a function from your 02 container and make it `constexpr` where possible; prove compile-time evaluation with `static_assert`.
2. Write a small compile-time-only utility using variadic templates (e.g. a compile-time type list, or a compile-time hash of a string literal).
3. Constrain your 02 container's template parameter with a C++20 concept (e.g. require movable, or require `operator==`); if you have an old SFINAE-based constraint, write it both ways and compare.
4. Implement one CRTP-based static-polymorphism example against a slice of your 05 ECS or a small new example, and get it doing what a virtual function did in 05 — without a vtable.
5. Use variadic templates + fold expressions to write a small type-safe variadic function (e.g. an event dispatcher or a `print_all(args...)`).
6. Write a short note: for each technique used here, would you reach for it in production code, or is it "know it exists, rarely reach for it"?

## Self-check questions

- What's the actual difference between an SFINAE-constrained template and a concept-constrained one — not just syntax, but what happens to the error message when a bad type is passed?
- Walk through what `if constexpr` does differently from a runtime `if` inside a template, and give an example where the runtime version wouldn't even compile.
- Given you benchmarked virtual dispatch vs data-oriented ECS in 05, where does CRTP sit on that spectrum — what do you gain and lose vs. each?
- What's a concrete production risk of overusing template metaprogramming (compile time blowup, unreadable error walls, hard-to-debug instantiations) — where's your personal line?

## Suggested tools/references

- cppreference: [constexpr](https://en.cppreference.com/w/cpp/language/constexpr), [Concepts](https://en.cppreference.com/w/cpp/language/constraints), [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae), [Parameter pack](https://en.cppreference.com/w/cpp/language/parameter_pack), [CRTP](https://en.cppreference.com/w/cpp/language/crtp)
- Headers: `<type_traits>`, `<concepts>`, `<utility>`
- [godbolt.org](https://godbolt.org) — inspect what your templates actually compile to, and compare CRTP vs virtual-call assembly
- Compile with `-std=c++20`; watch build times as you add metaprogramming, not just correctness

## Notes about concepts

- constexpr/consteval: Use constexpr for small, pure-arithmetic functions where compile-time evaluation is a real 
possibility (e.g. pnext), since it's unconditional in cost the compiler evaluates at compile time when it can, falls back 
to runtime otherwise. Reserve consteval for fixed, closed-form computations (e.g. filling constants into a known formula) 
where runtime evaluation should never be permitted at all.
- if constexpr: Useful when a template function needs to branch on type-dependent logic without every branch being forced 
to compile for every instantiation. Example: if constexpr (std::same_as<E, int>) means that branch only compiles when E is 
actually int. This is valuable when building systems where different types need mostly shared logic but a few type-specific 
cases, since it keeps that divergence in one function instead of splitting into separate overloads.
- C++20 concepts: A strong, production-worthy tool — they let you narrow a template's accepted types down to specific 
structural/behavioral requirements, while keeping debugging cheap: when something doesn't satisfy a constraint, the 
compiler tells you exactly what and why, instead of leaving you to reverse-engineer a wall of substitution-failure noise.
- Template specialization (full): Rarely reached for directly in my own code — more of a "know it exists and how the 
standard library uses it internally (e.g. is_integral)" tool than something I'd write regularly myself.
- Variadic templates + fold expressions: A genuine production tool — they let you write functions/classes that accept an 
arbitrary number of arguments of varying types, with real compile-time type safety, which is essential for things like the 
event dispatcher's subscribeMany.
- CRTP: Niche, "know it exists" tool rather than a regular production reach — only useful when you have a small, fixed, 
known set of concrete types and specifically want to avoid vtable overhead. Most systems (like the event dispatcher) need 
runtime flexibility (unknown types registered dynamically) far more than they need to save a vtable lookup, which rules 
CRTP out by design, not by preference.
