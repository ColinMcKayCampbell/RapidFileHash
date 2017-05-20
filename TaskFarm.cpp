#include "Tasks.h"
#include <mutex> // Shared resource protection
#include <string> 
#include <vector>
#include <queue>
#include <iostream>
#include <atomic>

using namespace std;
mutex taskQueueProtect;

void ThreadPool::AddTask(string a, uint64_t b, ThreadPool* c) // Add hash Task
	{
		unique_lock<mutex> lock(taskQueueProtect);
		TaskQueue.push(new HashTask(a, b, c)); // Protect TaskQueue while adding a HashTask
	}
void ThreadPool::AddTask(string a, uint64_t b, int c, string d) // Add write Task
{
	unique_lock<mutex> lock(taskQueueProtect);
	TaskQueue.push(new WriteTask(a, b, c, d)); // Protect TaskQueue while adding a HashTask
}

int ThreadPool::GetSize()
	{
	unique_lock<mutex> lock(taskQueueProtect);
	return TaskQueue.size();
	}

void ThreadPool::Execute()
	{
		vector<thread> ThreadQueue; // Initialise Thread Queue
	
		if (!GetSize())
		{
			cout << "No files found, quitting...\n";
			return;
		}

		int threadCount = coreCount;
		
		if(threadCount > GetSize())
		{
			threadCount = GetSize();
		}
		cout << GetSize() << " files found.\nHashing...\n";
		for (int x = 0; x < threadCount; x++) { // Create a thread for each core
			ThreadQueue.push_back(thread(mem_fun(&ThreadPool::Worker), this)); // Push thread objects to back of queue
		}
		for (auto &thread : ThreadQueue) thread.join();
	}


void ThreadPool::Worker()
	{
	queue<Task *> WorkerQueue;
		while (true)// While tasks remain
		{
		
			{
				unique_lock<mutex> lock(taskQueueProtect);
		
				if (TaskQueue.empty()) // Check for not empty
				{
				
					return;
				}
			
				WorkerQueue.push(TaskQueue.front()); // Take first task
		
				TaskQueue.pop(); // Remove task from front of queue
		
			}
		
			WorkerQueue.front()->run(); //  Run first task
			WorkerQueue.pop(); // Remove task
			

		}
		
	}
