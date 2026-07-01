#pragma once
#include "order.hpp"

namespace NanoMatch {
    struct alignas(64) PriceLevel {
        int64_t price;
        uint32_t total_volume;
        uint32_t order_count;
        
        Order* head;
        Order* tail;

        PriceLevel(int64_t p = 0) : price(p), total_volume(0), order_count(0), head(nullptr), tail(nullptr) {}

        // O(1) insertion at the back of the queue
        void add_order(Order* order) {
            total_volume += order->quantity;
            order_count++;
            order->next = nullptr;
            order->prev = tail;
            
            if (tail) {
                tail->next = order;
            } else {
                head = order;
            }
            tail = order;
        }

        // O(1) removal
        void remove_order(Order* order) {
            total_volume -= order->quantity;
            order_count--;
            
            if (order->prev) order->prev->next = order->next;
            else head = order->next;
            
            if (order->next) order->next->prev = order->prev;
            else tail = order->prev;
        }
    };
    static_assert(sizeof(PriceLevel) == 64, "PriceLevel must be 64-byte aligned");
}