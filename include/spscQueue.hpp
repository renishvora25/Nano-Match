#pragma once
#include <atomic>
#include <vector>
#include <cstddef>

namespace NanoMatch {
    template <typename T>
    class SPSCQueue {
    private:
        std::vector<T> buffer;
        const size_t mask;

        // Force head and tail onto separate cache lines to prevent false sharing
        alignas(64) std::atomic<size_t> head{0}; // Written by Producer
        alignas(64) std::atomic<size_t> tail{0}; // Written by Consumer

    public:
        SPSCQueue(size_t capacity) : buffer(capacity), mask(capacity - 1) {}

        bool push(const T& item) {
            size_t current_head = head.load(std::memory_order_relaxed);
            size_t next_head = (current_head + 1) & mask;

            if (next_head == tail.load(std::memory_order_acquire)) {
                return false; 
            }

            buffer[current_head] = item;
            head.store(next_head, std::memory_order_release);
            return true;
        }

        bool pop(T& item) {
            size_t current_tail = tail.load(std::memory_order_relaxed);

            if (current_tail == head.load(std::memory_order_acquire)) {
                return false; 
            }

            item = buffer[current_tail];
            tail.store((current_tail + 1) & mask, std::memory_order_release);
            return true;
        }
    };
}