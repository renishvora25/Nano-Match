#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

namespace NanoMatch {
    template <typename T>
    class MemoryPool {
    private:
        std::vector<T> arena;           // The contiguous memory block
        std::vector<uint32_t> freelist; // Stack of available indices
        
    public:
        MemoryPool(size_t capacity) {
            arena.resize(capacity);
            freelist.reserve(capacity);
            for (uint32_t i = 0; i < capacity; i++) {
                freelist.push_back(capacity - 1 - i); 
            }
        }

        // O(1) acquisition
        T* allocate() {
            if (freelist.empty()) [[unlikely]] {
                throw std::bad_alloc(); 
            }
            uint32_t index = freelist.back();
            freelist.pop_back();
            return &arena[index];
        }

        // O(1) release back to the pool
        void deallocate(T* ptr) {
            // Calculate distance from start of arena to get the index
            uint32_t index = static_cast<uint32_t>(ptr - &arena[0]);
            freelist.push_back(index);
        }
    };
}