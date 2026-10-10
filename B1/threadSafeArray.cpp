#include "threadSafeArray.h"

ThreadSafeArray::ThreadSafeArray(ull size): bools(size, true)
//times(size) 
{
}

bool ThreadSafeArray::get(ull i) {
	std::lock_guard<std::mutex> lock(m);
	return bools[i];
}

//void ThreadSafeArray::setValue(ull i, bool b, timetype t) {
//	std::lock_guard<std::mutex> lock(m);
//	bools[i] = b;
//	times[i] = t;
//}

void ThreadSafeArray::setFalse(ull i) {
	std::lock_guard<std::mutex> lock(m);
	bools[i] = false;
}