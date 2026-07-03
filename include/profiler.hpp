#pragma once
#include <cstdint>

// Architecture detection for the hardware cycle counter
#if defined(__aarch64__) || defined(_M_ARM64)
    // Apple Silicon / ARM architecture
    static inline uint64_t hardware_cycle_counter() {
        uint64_t val;
        __asm__ volatile("mrs %0, cntvct_el0" : "=r" (val));
        return val;
    }
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
    // Intel / AMD x86 architecture
    #ifdef _MSC_VER
        #include <intrin.h>
    #else
        #include <x86intrin.h>
    #endif

    static inline uint64_t hardware_cycle_counter() {
        unsigned int aux;
        return __rdtscp(&aux);
    }
#else
    #include <chrono>
    static inline uint64_t hardware_cycle_counter() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
#endif

namespace NanoMatch {
    class Profiler {
    public:
        // Reads the hardware cycle counter directly from the CPU
        static inline uint64_t rdtsc() {
            return hardware_cycle_counter();
        }
    };
}