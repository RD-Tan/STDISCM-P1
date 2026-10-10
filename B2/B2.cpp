#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <format>
#include <thread>
#include <vector>
#include <atomic>
#include <semaphore>
#include <iostream>
#include "readconfig.h"
#include "threadpool.h"


int main(const int argc, const char* argv[]) {
	int readThreadCount;
	ull until, dividend = 2;
	if (readConfig("config.txt", &readThreadCount, &until)) {
		exit(1);
	}
	auto start = std::chrono::steady_clock::now();


	ThreadPool tp(readThreadCount);

	while (dividend < until) {
		bool isPrime = tp.checkIsPrime(dividend);
		printf("%llu is %s.\n", dividend, isPrime ? "prime" : "not prime");
		dividend++;
	}

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Execution time: " << duration << " ms\n";


	return 0;
}