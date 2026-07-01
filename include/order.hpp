#pragma once
#include <cstdint>

namespace NanoMatch {
    enum class Side : uint8_t {
         Buy = 0,
          Sell = 1 
    };
    enum class OrderType : uint8_t { 
        Limit = 0, 
        Market = 1 
    };

    struct alignas(64) Order {
        uint64_t order_id;
        int64_t  price;      
        uint64_t timestamp;  //RDTSC cycle count
        uint32_t quantity;
        Side     side;
        OrderType type;
        
        // Intrusive list pointers for O(1) removal without heap allocations
        Order* prev;
        Order* next;
    };
    static_assert(sizeof(Order) == 64, "Order must fit exactly in one cache line");
}