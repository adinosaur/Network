
#ifndef NETWORK_THREAD_SAFE_QUEUE_H_
#define NETWORK_THREAD_SAFE_QUEUE_H_

#include <mutex>
#include <queue>

template <typename T>
class ThreadSafeQueue 
{
public:
	void push(const T& val)
	{
		mtx_.lock();
		queue_.push(val);
		mtx_.unlock();
	}

	void pop()
	{
		mtx_.lock();
		queue_.pop();
		mtx_.unlock();
	}

	T front()
	{
		return queue_.front();
	}

	bool empty()
	{
		return queue_.empty();
	}

	typename std::queue<T>::size_type size()
	{
		return queue_.size();
	}
private:
	std::mutex mtx_;
	std::queue<T> queue_;
};

#endif