#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <semaphore>
#include <atomic>
#include "readconfig.h"

// Global pointer for the semaphore so worker threads can access it
std::counting_semaphore<1000>* threadCountSem = nullptr;

void run(std::atomic<bool>* isPrime, unsigned long long int dividend, unsigned long long int divisor) {
    // If another thread already found it's not prime, skip unnecessary work
    if (!isPrime->load()) {
        threadCountSem->release();
        return;
    }

    if (dividend % divisor == 0) {
        *isPrime = false;
    }
    threadCountSem->release();
}

int main(int argc, char* argv[]) {
    int threadCount;
    unsigned long long until, dividend = 2;

    if (readConfig("config.txt", &threadCount, &until)) {
        exit(1);
    }

    auto start = std::chrono::steady_clock::now();

    // Initialize semaphore with the thread count read from config
    threadCountSem = new std::counting_semaphore<1000>(threadCount);

    while (dividend <= until) {
        // Handle base cases quickly
        if (dividend == 2 || dividend == 3) {
            printf("%llu is prime.\n", dividend);
            dividend++;
            continue;
        }

        std::vector<std::thread> threads;
        unsigned long long divisor = 2;
        std::atomic<bool> isPrime{ true };

        // Spawn threads to test divisors up to dividend / 2
        while (divisor <= dividend / 2 && isPrime.load()) {
            threadCountSem->acquire();
            threads.push_back(std::thread(run, &isPrime, dividend, divisor));
            divisor++;
        }

        for (auto& t : threads) {
            t.join();
        }

        printf("%llu is %s.\n", dividend, isPrime.load() ? "prime" : "not prime");
        dividend++;
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Execution time: " << duration << " ms\n";

    delete threadCountSem;
    return 0;
}