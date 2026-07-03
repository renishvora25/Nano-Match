#include "../include/order.hpp"
#include "../include/priceLevel.hpp"
#include "../include/memoryPool.hpp"
#include <map>
#include <algorithm> 

namespace NanoMatch {
    class OrderBook {
    private:
        // Bids sorted descending (highest price first), Asks sorted ascending
        std::map<int64_t, PriceLevel, std::greater<int64_t>> bids;
        std::map<int64_t, PriceLevel, std::less<int64_t>> asks;
        
        MemoryPool<Order> order_pool;

    public:
        OrderBook(size_t pool_capacity) : order_pool(pool_capacity) {}

        void process_limit_order(int64_t price, uint32_t qty, Side side, uint64_t id) {
            Order* new_order = order_pool.allocate();
            new_order->price = price;
            new_order->quantity = qty;
            new_order->side = side;
            new_order->order_id = id;

            if (side == Side::Buy) {
                match_buy_order(new_order);
            } else {
                match_sell_order(new_order);
            }
        }

    private:
        void match_buy_order(Order* order) {
            auto it = asks.begin();
            while (it != asks.end() && order->quantity > 0) {
                PriceLevel& level = it->second;
                
                // If the lowest ask is higher than our buy price, stop matching
                if (level.price > order->price) break;

                // Match against resting orders at this price level
                Order* resting = level.head;
                while (resting && order->quantity > 0) {
                    uint32_t fill_qty = std::min(order->quantity, resting->quantity);
                    order->quantity -= fill_qty;
                    resting->quantity -= fill_qty;

                    if (resting->quantity == 0) {
                        Order* next_resting = resting->next;
                        level.remove_order(resting);
                        order_pool.deallocate(resting);
                        resting = next_resting;
                    } else {
                        break;
                    }
                }
                
                if (level.order_count == 0) {
                    it = asks.erase(it);
                } else {
                    ++it;
                }
            }

            // If quantity remains, add it to the bid book
            if (order->quantity > 0) {
                bids[order->price].price = order->price;
                bids[order->price].add_order(order);
            } else {
                order_pool.deallocate(order);
            }
        }
        
        void match_sell_order(Order* order) {
            auto it = bids.begin();
            while (it != bids.end() && order->quantity > 0) {
                PriceLevel& level = it->second;
                
                // If the highest bid is lower than our sell price, stop matching
                if (level.price < order->price) break;

                // Match against resting orders at this price level
                Order* resting = level.head;
                while (resting && order->quantity > 0) {
                    uint32_t fill_qty = std::min(order->quantity, resting->quantity);
                    order->quantity -= fill_qty;
                    resting->quantity -= fill_qty;

                    // If the resting order is fully filled, remove and deallocate it
                    if (resting->quantity == 0) {
                        Order* next_resting = resting->next;
                        level.remove_order(resting);
                        order_pool.deallocate(resting);
                        resting = next_resting;
                    } else {
                        break;
                    }
                }
                
                // If the entire price level is now empty, remove it from the map
                if (level.order_count == 0) {
                    it = bids.erase(it);
                } else {
                    ++it;
                }
            }

            // If the incoming sell order still has remaining quantity, add it to the asks book
            if (order->quantity > 0) {
                asks[order->price].price = order->price;
                asks[order->price].add_order(order);
            } else {
                order_pool.deallocate(order);
            }
        }
    };
}