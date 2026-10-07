#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <format>
#include <thread>
#include <vector>

bool isPrime(unsigned long long int x) {
	if (x == 1) return false;
	unsigned long long int i = 2;
	unsigned long long int max = x / 2;
	do {
		if (x % i == 0)
			return false;
		i++;
	} while (i < max);
	return true;
}

void run(int id, unsigned long long int lowerb, unsigned long long int upperb) {
	do
	{
		bool isCurrPrime = isPrime(lowerb);
		auto now = std::chrono::system_clock::now();
		std::string readable_time = std::format("{:%Y-%m-%d %H:%M:%S}", now);

		if (isCurrPrime) {
			printf("%s [%d]: %llu is prime.\n", readable_time.c_str(), id, lowerb);
		}
		else {
			printf("%s [%d]: %llu is not prime.\n", readable_time.c_str(), id, lowerb);
		}
		lowerb++;
	} while (lowerb <= upperb);
}


int main(const int argc, const char *argv[]) {
	printf("argument count: %d\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("[%d]: %s\n",i, argv[i]);
	}

	int threadCount = strtol(argv[1], NULL, 10);
	if (errno == ERANGE) {
		perror("value given for thread exceeds int.");
		return 1;
	}
	else if (threadCount < 1) {
		perror("thread count given is zero or negative. Must be positive.");
		return 1;
	}

	unsigned long long until = strtoull(argv[2], NULL, 10) + 1;
	if (errno == ERANGE) {
		perror("value given for upper range exceeds unsigned long long int.");
		return 1;
	}
	else if (until < 1) {
		perror("upper range given is zero or negative. Must be positive.");
		return 1;
	}

	std::vector<std::thread> threads;
	unsigned long long interval = until / threadCount;
	for (int i = 0; i < threadCount; i++) {
		unsigned long long lowerb = interval * i;
		unsigned long long upperb = interval * (i + 1) - 1;
		if (i == threadCount - 1) {
			upperb = upperb + (until % threadCount);
		}
		threads.push_back(std::thread(run, i, lowerb, upperb));
	}

	for (auto &t : threads) {
		t.join();
	}

	return 0;
}