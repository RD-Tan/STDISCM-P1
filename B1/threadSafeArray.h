#pragma once
#include <mutex>
#include <vector>
#include <chrono>
#define ull unsigned long long int
#define timetype std::chrono::time_point<std::chrono::system_clock>

class ThreadSafeArray {
	std::mutex m;
	std::vector<bool> bools;
	//std::vector<timetype> times;

public:

	ThreadSafeArray();
	ThreadSafeArray(ull size);
	bool get(ull i);
	void setValue(ull i, bool b, timetype t);
	void setFalse(ull i);
};