#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <deque>
//#include <c++/4.8.3/condition_variable>

/*
* A thread pool is a group of pre-instantiated, idle worker threads which stand ready to be given work.
* These are preferred over instantiating new threads for each task when there is a large number of short tasks to be done rather than a small number of long ones.
* This prevents having to incur the overhead of creating a thread a large number of times.
*/

class ThreadPool; // forward declare

using namespace std;

class Worker {
public:
	explicit Worker(ThreadPool &s) : pool_(s) { }
	void operator()() const;
private:
	ThreadPool & pool_;
};

class ThreadPool {
public:
	explicit ThreadPool(size_t threads);
	template<class F> void enqueue(F f);

	bool no_tasks() const;
	~ThreadPool();
private:
	friend class Worker;

	mutex m_;
	condition_variable cv_;

	vector<thread> workers_;
	deque<function<void()>> tasks_;

	mutex queue_mutex_;
	bool stop_;
};

inline void Worker::operator()() const
{
	while (true)
	{
		unique_lock<mutex> locker(pool_.queue_mutex_);

		if (pool_.stop_)
			return;

		// wait for condition variable
		while (pool_.tasks_.empty())
		{
			if (pool_.stop_)
				return;

			pool_.cv_.wait(locker);
		}

		const auto task = pool_.tasks_.front(); // top/front
		pool_.tasks_.pop_front(); // pop
		locker.unlock(); // unlock
		task(); // do task
	}
}

inline ThreadPool::ThreadPool(const size_t threads) : stop_(false)
{
	for (size_t i = 0; i < threads; ++i)
		workers_.emplace_back(Worker(*this));
}

inline bool ThreadPool::no_tasks() const
{
	return tasks_.empty();
}

inline ThreadPool::~ThreadPool()
{
	// signify threads to stop
	stop_ = true; // stop all threads

	// Unblocks all threads currently waiting for the cv
	cv_.notify_all();

	// wait for all threads to join (and thus finish)
	for (auto &thread : workers_)
		thread.join();
}

template<class F>
void ThreadPool::enqueue(F f)
{
	// make unique lock with the mutex
	unique_lock<mutex> lock(queue_mutex_);
	tasks_.push_back(function<void()>(f));
	cv_.notify_one(); // wake up one of the blocked consumers
}