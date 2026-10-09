#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <format>
#include <thread>
#include <vector>
#include <atomic>
#include <semaphore>
#include "readconfig.h"
#include "threadpool.h"


int main(const int argc, const char* argv[]) {
	int readThreadCount;
	ull until, dividend = 2;
	if (readConfig("config.txt", &readThreadCount, &until)) {
		exit(1);
	}


	ThreadPool tp(readThreadCount);
	
	while (dividend < until) {
		bool isPrime = tp.checkIsPrime(dividend);
		printf("%llu is %s.\n", dividend, isPrime ? "prime" : "not prime");
		dividend++;
	}



	return 0;
}