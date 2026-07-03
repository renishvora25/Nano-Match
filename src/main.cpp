#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include "../include/order.hpp"
#include "../include/spscQueue.hpp"
#include "./orderBook.cpp" 

using namespace NanoMatch;

// Global flag to cleanly shut down the engine
std::atomic<bool> engine_running{true};

// 1. The Producer (Simulating a network feed)
void network_feed_producer(SPSCQueue<Order>& queue, int num_orders) {
    std::cout << "[Producer] Thread started. Generating " << num_orders << " orders...\n";
    
    for (int i = 0; i < num_orders; ++i) {
        Order new_order;
        new_order.order_id = i;
        new_order.price = 1000000 + (i % 50); // Arbitrary price variation
        new_order.quantity = 100;
        new_order.side = (i % 2 == 0) ? Side::Buy : Side::Sell;
        
        // Spin-wait if the lock-free queue is full
        while (!queue.push(new_order)) {
            std::this_thread::yield(); 
        }
    }
    
    std::cout << "[Producer] Finished sending orders.\n";
    engine_running.store(false, std::memory_order_release);
}

// 2. The Consumer (The Core Matching Engine)
void matching_engine_consumer(SPSCQueue<Order>& queue, OrderBook& book) {
    std::cout << "[Consumer] Thread started. Polling for orders...\n";
    
    Order incoming_order;
    int orders_processed = 0;
    
    // The Hot Path Spin-Loop
    while (true) {
        if (queue.pop(incoming_order)) {
            book.process_limit_order(
                incoming_order.price, 
                incoming_order.quantity, 
                incoming_order.side, 
                incoming_order.order_id
            );
            orders_processed++;
        } else {
            // The queue is currently empty. Are we shutting down?
            if (!engine_running.load(std::memory_order_acquire)) {
                while (queue.pop(incoming_order)) {
                    book.process_limit_order(
                        incoming_order.price, 
                        incoming_order.quantity, 
                        incoming_order.side, 
                        incoming_order.order_id
                    );
                    orders_processed++;
                }
                break; 
            }
        }
    }
    
    std::cout << "[Consumer] Engine stopped. Total orders processed: " << orders_processed << "\n";
}

int main() {
    std::cout << "Starting Nano Match Engine...\n";

    SPSCQueue<Order> order_queue(1024 * 1024); 
    
    const int TOTAL_ORDERS = 5'000'000;
    OrderBook book(TOTAL_ORDERS + 100); 

    std::thread consumer_thread(matching_engine_consumer, std::ref(order_queue), std::ref(book));
    std::thread producer_thread(network_feed_producer, std::ref(order_queue), TOTAL_ORDERS);

    producer_thread.join();
    consumer_thread.join();

    std::cout << "Engine successfully shut down.\n";
    return 0;
}