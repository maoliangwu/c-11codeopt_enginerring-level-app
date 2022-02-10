// ThreadPool_les1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "ThreadPool.h"
void TestThreadPool()
{
    ThreadPool pool;
    std::thread thread1([&pool]()
    {
        for (int i = 0; i < 10; i++)
        {
            auto thid = this_thread::get_id();
            pool.AddTask([thid] {
                cout << "同步层线程1的线程ID： " << thid << endl;
            });
        }
	});
	std::thread thread2([&pool]()
	{
		for (int i = 0; i < 10; i++)
		{
			auto thid = this_thread::get_id();
			pool.AddTask([thid] {
				cout << "同步层线程2的线程ID： " << thid << endl;
			});
		}
	});
	this_thread::sleep_for(std::chrono::seconds(2));
	getchar();
	pool.Stop();
	thread1.join();
	thread2.join();
}
int main()
{
    std::cout << "Hello World!\n";
}
