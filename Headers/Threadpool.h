#pragma once

#include <condition_variable>
#include <cstddef>
#include <c++/4.8.3/thread>
#include <queue>

class ThreadPool
{
public:
    using Task = std::function<void()>;

    explicit ThreadPool(std::size_t thread_amount)
    {
        start(thread_amount);
    }

    ~ThreadPool()
    {
        stop();
    }

    void enqueue(Task task)
    {
        {
            std::unique_lock<std::mutex> lock{event_mutex};
            tasks.emplace((std::move(task)));
        }

        event.notify_one();
    }

private:
    // Vector of threads
    std::vector<std::thread> threads;
    // Event checker
    std::condition_variable event;
    // Mutex
    std::mutex event_mutex;

    bool stopping = false;
    std::queue<Task> tasks;

    void start(std::size_t thread_amount)
    {
        for (auto i = 0; i < thread_amount; i++)
        {
            threads.emplace_back([&] {
                while(true)
                {
                    Task task;

                    // Extra scope so mutex isn't locked while task is executing
                    {
                        std::unique_lock<std::mutex> lock{event_mutex};

                        event.wait(lock, [&] { return stopping || !tasks.empty(); });

                        if (stopping){
                            break;
                        }
                        else{
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                    }

                    task();
                }
            });
        }
    }

    void stop()
    {
        {
            std::unique_lock<std::mutex> lock(event_mutex);
            stopping = true;
        }

        event.notify_all();

        for (auto &thread : threads)
        {
            thread.join();
        }
    }
};