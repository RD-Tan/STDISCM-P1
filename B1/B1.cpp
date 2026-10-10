#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <format>
#include <thread>
#include <vector>
#include <iostream>
#include "readconfig.h"
#include "threadSafeArray.h"



bool isPrime(unsigned long long int x) {
	if (x == 1) return false;
	if (x == 2 || x == 3) return true;
	unsigned long long int i = 2;
	unsigned long long int max = x / 2;
	while (i <= max)
	{
		if (x % i == 0)
			return false;
		i++;
	}
	return true;
}

void run(int id, unsigned long long int lowerb, unsigned long long int upperb, ThreadSafeArray *isPrimes) {
	do
	{
		bool isCurrPrime = isPrime(lowerb);
		if (!isCurrPrime) {
			isPrimes->setFalse(lowerb - 1);
		}
		lowerb++;
	} while (lowerb <= upperb);
}


int main(const int argc, const char* argv[]) {
	int threadCount;
	unsigned long long until;
	if (readConfig("config.txt", &threadCount, &until)) {
		exit(1);
	}
	auto start = std::chrono::steady_clock::now();

	ThreadSafeArray isPrimes = ThreadSafeArray(until);

	std::vector<std::thread> threads;
	
	unsigned long long interval = until / threadCount;


	for (int i = 0; i < threadCount; i++) {
		unsigned long long lowerb = interval * i + 1;
		unsigned long long upperb = interval * (i + 1) - 1;
		if (i == threadCount - 1) {
			upperb = upperb + (until % threadCount);
		}
		threads.push_back(std::thread(run, i, lowerb, upperb, &isPrimes));
	}

	for (auto& t : threads) {
		t.join();
	}

	for (ull i = 0; i < until; i++) {
		bool isPrime = isPrimes.get(i);
		printf("%llu is %s.\n", i + 1, isPrime ? "prime" : "not prime");
	}

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Execution time: " << duration << " ms\n";

	return 0;
}