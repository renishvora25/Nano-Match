NanoMatch is a single-threaded, ultra-low latency order matching engine focused on performance, memory efficiency, and systems-level programming.

The project is being built to demonstrate concepts used in real-world trading systems such as:

Price-Time Priority (FIFO)
Zero dynamic memory allocation on the hot path
Custom Memory Pool (Slab Allocator)
Intrusive Linked Lists
Lock-Free SPSC Ring Buffer
Cache-Line Alignment
RDTSC-based Performance Benchmarking