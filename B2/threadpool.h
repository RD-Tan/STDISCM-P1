#pragma once
#include <atomic>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>
#include <memory>
#include <queue>
#include <condition_variable>
#include <functional>
#define ull unsigned long long int

class ThreadPool
{
private:
    std::vector<std::jthread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable cvTaskAvailable;
    std::condition_variable cvTaskFinished;

    std::atomic<bool> stopPool{ false };
    std::atomic<size_t> activeTasks{ 0 };

    void workerLoop();

public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();
    void enqueue(std::function<void()> task);
    void clearQueue();
    void waitAll();
    bool checkIsPrime(ull n);
};