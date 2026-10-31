# 03 — Thread Pool

A job-queue thread pool — workers pull tasks off a shared queue and execute them concurrently — wired into the chat server from 01 so client messages are handled off the main thread.

## Prerequisites

Finish 01 (TCP Chat Server) and 02 (Generic Container Library).

- From 02, you have `RingBuffer<T>` — fixed-capacity, non-thread-safe, with synchronous `push()` (returns `false` if full) and `pop()` (returns `std::optional<T>`, `nullopt` if empty). Neither method blocks or knows about other threads.
- From 01, you have a working one-thread-per-client chat server, whose shared `std::vector<std::shared_ptr<Connection>>` client list is **already** `std::mutex`-protected (see `clientThread`'s locked sections around `push_back`, the broadcast copy, and `erase`) — that part of the thread-safety work is already done, from before this project existed.

You'll decide whether the job queue wraps your `RingBuffer<T>` or `std::queue`, and how to wire the pool into 01's server given its existing per-connection-thread architecture (see milestone 5).

## Learning objectives

- `std::thread`: creation, joining, detaching
- `std::mutex`, `std::atomic`
- `std::condition_variable` for wait/notify job signaling
- RAII-based locking: `std::lock_guard`, `std::unique_lock`
- Recognizing and avoiding data races
- Bounded vs. unbounded queues: backpressure (blocking `push` when full) vs. unlimited growth, and what changes when you wrap an existing non-thread-safe container (`RingBuffer<T>`) instead of designing the queue's locking in from scratch

## Why this project

You've done `pthreads` in C — the primitives here map closely (`pthread_mutex_t` → `std::mutex`, `pthread_cond_t` → `std::condition_variable`), but C++ wraps locking in RAII so you can't forget to unlock, even on an exception path. This project gives you a measurable before/after (serial vs pooled chat-server throughput) and produces the job system the capstone (06) reuses for simulation/network work.

## Core concepts to research first

- `std::thread` lifecycle: joinable vs joined vs detached, what happens if a `thread` destructs while still joinable
- `std::mutex` vs `std::atomic` — when a lock is required vs when an atomic suffices
- `std::lock_guard` vs `std::unique_lock` (the latter's flexibility: deferred lock, manual unlock, works with `condition_variable`)
- `std::condition_variable`: spurious wakeups, why you always wait with a predicate
- data race vs race condition (they are not the same thing)
- `std::packaged_task` / `std::future` (optional, for returning results from jobs)
- false sharing / cache-line contention (brief awareness, not deep dive yet — that's project 05)

## Build milestones

1. Implement a thread-safe job queue: `push(job)` / `pop_blocking()` guarded by a mutex + condition_variable. Decide deliberately: reuse `RingBuffer<T>` as the backing store (bounded — decide what `push` does when it reports full: block for backpressure, or fail-fast and make the caller handle it) or switch to `std::queue` (unbounded, `push` always succeeds). Either way, `RingBuffer<T>`/`std::queue` itself stays non-thread-safe; the thread-safe queue class composes it with its own mutex + condition_variable rather than modifying the container.
2. Spin up N worker threads in the pool constructor; each runs a loop: wait for job, pop, execute, repeat.
3. Implement clean shutdown: a stop flag (`std::atomic<bool>` or condition-checked) that wakes all workers and joins every thread with no leaked/detached threads.
4. Write a standalone benchmark: submit M independent jobs, compare wall-clock time serial vs pooled across worker counts.
5. Wire the pool into the 01 chat server. Note the architecture mismatch first: 01 currently dedicates one blocking thread per client (parked in `recvMsg` most of the time), so naively handing a whole `clientThread` call to the pool as "one job" would just consume one pool worker per connected client for that connection's entire lifetime — a fixed-size pool fills up and stalls well before you run out of clients. Work out what the actual job boundary should be (e.g., the per-connection thread still blocks on `recvMsg`, but each *received message's* handling — formatting + broadcast — becomes the job submitted to the pool) before writing code.
6. Stress-test with many concurrent clients. 01's `vector<shared_ptr<Connection>>` is already mutex-protected from before this project (see Prerequisites) — confirm that protection still holds once the pool's worker threads, not just per-client threads, are touching shared state, and check for any *new* shared state the pool itself introduces (e.g., an active-job counter) that still needs its own synchronization.

## Self-check questions

- Why must a `condition_variable::wait` always be called with a predicate (the `wait(lock, predicate)` overload) instead of a bare wait-then-check?
- Explain, in terms of the mutex, exactly what `lock_guard`'s destructor does and why that makes it exception-safe compared to manual `lock()`/`unlock()`.
- If you wrap `RingBuffer<T>` instead of `std::queue`: what does your `push()` do when the ring buffer reports full, and why did you choose that behavior over the alternative?
- In 01's architecture, what exactly becomes "a job" once the pool is wired in — the whole client connection, or something smaller — and why does that choice matter for a *fixed-size* pool?
- What's a concrete data race your pool could have if you used `int` instead of `std::atomic<int>` for the active-job counter?

## Suggested tools/references

- cppreference: [std::thread](https://en.cppreference.com/w/cpp/thread/thread), [std::condition_variable](https://en.cppreference.com/w/cpp/thread/condition_variable), [std::unique_lock](https://en.cppreference.com/w/cpp/thread/unique_lock), [std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic)
- Headers: `<thread>`, `<mutex>`, `<condition_variable>`, `<atomic>`, `<queue>`, `<future>`
- Compile/link with `-pthread`, `-std=c++20`
- ThreadSanitizer (`-fsanitize=thread`) to confirm you actually have no data races — don't just eyeball it
