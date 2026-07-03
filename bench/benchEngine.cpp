#include "../src/orderBook.cpp"
#include "../include/profiler.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace NanoMatch;

int main() {
    const size_t NUM_ORDERS = 10'000'000;
    
    std::cout << "Allocating memory pool for " << NUM_ORDERS << " orders...\n";
    OrderBook book(NUM_ORDERS + 100); 

    std::vector<int64_t> prices(NUM_ORDERS);
    std::vector<Side> sides(NUM_ORDERS);
    
    for(size_t i = 0; i < NUM_ORDERS; ++i) {
        prices[i] = 1000000 + (i % 100); 
        sides[i] = (i % 2 == 0) ? Side::Buy : Side::Sell;
    }

    std::cout << "Starting high-frequency benchmark (Hot Path)...\n";

    uint64_t start_cycles = Profiler::rdtsc();

    for (size_t i = 0; i < NUM_ORDERS; ++i) {
        book.process_limit_order(prices[i], 100, sides[i], i);
    }

    uint64_t end_cycles = Profiler::rdtsc();
    
    uint64_t total_cycles = end_cycles - start_cycles;
    
    // Calculate metrics
    double cycles_per_order = static_cast<double>(total_cycles) / NUM_ORDERS;
    
    const double CPU_GHZ = 3.2; 
    double latency_ns = cycles_per_order / CPU_GHZ;
    
    double orders_per_second = (NUM_ORDERS / (total_cycles / (CPU_GHZ * 1'000'000'000)));

    std::cout << "\n=== BENCHMARK RESULTS ===\n";
    std::cout << "Total Orders Processed: " << NUM_ORDERS << "\n";
    std::cout << "Total CPU Cycles:       " << total_cycles << "\n";
    std::cout << "-------------------------\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Throughput:             " << (orders_per_second / 1'000'000) << " Million orders/sec\n";
    std::cout << "Latency:                " << latency_ns << " ns/order (" << cycles_per_order << " cycles)\n";
    std::cout << "=========================\n";

    return 0;
}