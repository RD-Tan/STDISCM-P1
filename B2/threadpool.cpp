#include "threadpool.h"

ThreadPool::ThreadPool(size_t threads) {
    workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopPool = true;
    }
    cvTaskAvailable.notify_all();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
        activeTasks++;
    }
    cvTaskAvailable.notify_one();
}

void ThreadPool::waitAll() {
    std::unique_lock<std::mutex> lock(queueMutex);
    cvTaskFinished.wait(lock, [this]() {
        return tasks.empty() && activeTasks == 0;
        });
}

void ThreadPool::clearQueue() {
    std::lock_guard<std::mutex> lock(queueMutex);

    // Decrement activeTasks by the number of discarded tasks
    activeTasks -= tasks.size();

    std::queue<std::function<void()>> emptyQueue;
    std::swap(tasks, emptyQueue);

    if (activeTasks == 0) {
        cvTaskFinished.notify_all();
    }
}

bool ThreadPool::checkIsPrime(ull n) {
    std::atomic<bool> isPrime{ true };

    if (n < 2) return false;
    if (n == 2 || n == 3)
    {
        return true;
    }

    for (ull divisor = 3; divisor * divisor <= n; divisor++) {
        if (!isPrime.load(std::memory_order_relaxed)) {
            break;
        }

        enqueue([this, &isPrime, n, divisor]() {
            if (!isPrime.load(std::memory_order_relaxed)) return;

            if (n % divisor == 0) {
                isPrime.store(false, std::memory_order_relaxed);
                clearQueue();
            }
            });
    }

    waitAll();
    return isPrime.load();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            cvTaskAvailable.wait(lock, [this]() {
                return stopPool || !tasks.empty();
                });

            if (stopPool && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            activeTasks--;
            if (tasks.empty() && activeTasks == 0) {
                cvTaskFinished.notify_all(); // Signal waitAll()
            }
        }
    }
}