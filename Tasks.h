#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;
class Task // Master Task class deckaration
{
public:
	//virtual ~Task();
	virtual void run() = 0;

};

class ThreadPool // Task farmer class
{
public:
	void SetReady(bool rdy);
	void AddTask(string a, uint64_t b, ThreadPool* c); // Add hash task to queue
	void AddTask(string a, uint64_t b, int c, string d); // Add write task to queue
	int GetSize(); // Get size of TaskQueue
	void Execute();// Run all tasks in queue

private:
	int coreCount = thread::hardware_concurrency(); // Get hardware CPU core count
	queue<Task *> TaskQueue; // Storage for tasks
	void Worker(); // Worker function 
	condition_variable TaskReady;
	mutex TaskCV;
	bool ready = false;
};

class HashTask : public Task { // Task class for file hashing
public:
	HashTask(string a, uint64_t b, ThreadPool * c); //  a - file path, b - file size, c - Thread farm object reference
	void run(); // Run file hashing task
private:
	string filePath; // Absolute file path
	uint64_t size; // File size in bytes (up to 64 bits)
	ThreadPool* farmRef;
};

class WriteTask : public Task // Task class for writing to disk
{
public:
	WriteTask(string a, uint64_t b, int c, string d); //  a - file path, b - file size, c - time taken to hash file, d - final file hash
	void run(); // Run disk write task

private:
	string filePath; // As in hash task
	uint64_t size; //  ^^
	int time_taken; // Time taken for hash task to run
	string hash_out; // File hash

};