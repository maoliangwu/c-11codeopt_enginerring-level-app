#pragma once
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
using namespace std;
template <typename T>
class SyncQueue
{
public:
	SyncQueue(int maxsize) :m_maxsize(maxsize), m_needstop(false)
	{

	}
	void Put(const T& x)
	{
		Add(x);
	}
	void Put(T&& x)
	{
		Add(std::forward<T>(x));
	}
	void Take(std::list<T>& list)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notEmpty.wait(locker, [this] {return m_needstop || NotEmpty(); });
		if (m_needstop)
			return;
		list = std::move(m_queue);
		m_notFull.notify_one();
	}
	void Take(T& t)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notEmpty.wait(locker, [this] {return m_needstop || NotEmpty(); });
		if (m_needstop)
			return;
		t = m_queue.front();
		m_queue.pop_front();
		m_notFull.notify_one();
	}
	void Stop()
	{
		{
			std::lock_guard<std::mutex> locker(m_mutex);
			m_needstop = true;
		}
		m_notFull.notify_all();
		m_notEmpty.notify_all();
	}

	bool Empty()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.empty();
	}
	bool Full()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.size() >= m_maxsize;
	}
	size_t Size()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.size();
	}
	int Count()//?
	{
		return m_queue.size();
	}
private:
	bool NotFull() const
	{
		bool full = m_queue.size() >= m_maxsize;
		if (full)
		{
			cout << "缓冲区慢，需要等待...." << endl;
		}
		return !full;
	}
	bool NotEmpty() const
	{
		bool empty = m_queue.empty();
		if (empty)
		{
			cout << "缓冲区空，需要等待...." << endl;
		}
		return !empty;
	}

	template <typename T>
	void Add(T&& x)
	{
		std::unique_lock<std::mutex>locker(m_mutex);
		m_notFull.wait(locker, [this] {return m_needstop || NotFull(); });
		if (m_needstop)
		{
			return;
		}
		m_queue.push_back(std::forward<T>(x));
		m_notEmpty.notify_one();
	}
private:
	std::list<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_notEmpty;
	std::condition_variable m_notFull;
	int m_maxsize;
	bool m_needstop;
};

