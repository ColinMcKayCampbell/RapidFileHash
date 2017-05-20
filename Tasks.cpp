//Task Class file

#include "Tasks.h"
#include "sha256.h" // SHA256 Hashing
#include <chrono> // Timing
#include <iostream> // Terminal I/O
#include <fstream> // File I/O
#include <mutex> // Shared resource protection

using namespace std;
// Timing code
using std::chrono::duration_cast;
using std::chrono::milliseconds;
typedef std::chrono::steady_clock the_clock;
// !Timing code
mutex diskWriterProtect; // Mutex for disk writing



HashTask::HashTask(string a, uint64_t b, ThreadPool* c) // HashTask constructor 
{
	filePath = a; 
	size = b; // File size
	farmRef = c;
}

void HashTask::run() // Run hashing task
	{
	the_clock::time_point start = the_clock::now(); // Start timer
	FILE * fs;
	FILE ** filePass = &fs; // Pointer to file pointer, required for fopen_s
	SHA256 hash; // Create SHA256 hashing object (from sha256.h)
	char * buffer;
	int bufferSize;
	int limit = 4096; // File read buffer limit in bytes.
	if (size > limit ) {
		 bufferSize = limit;
	}
	else // If file is smaller than limit, make buffer the size of the file.
	{
		 bufferSize = size; 
	}
	size_t result; 
	int time_taken;
	string hash_out;
	const char* path = filePath.c_str(); // Convert path string to c style strings for fopen_s
	fopen_s(filePass, path, "r"); // Open file in read only mode
	if (fs != NULL) { // If opened successfully
		
		buffer = (char*)malloc(bufferSize); // Allocate a buffer of char pointers
		if (buffer == NULL) { fputs("Malloc error.\n", stderr); exit(2); } // Throw error if malloc fails
		while (bufferSize - ftell(fs) > bufferSize) { // While there is still a full buffer left in file stream
			fread(buffer, 1, bufferSize, fs); // Read a buffer's worth of bytes
			hash.add(buffer, bufferSize); // Hash buffer
		}
		if(ftell(fs)!=size) // If the file stream has not all been accessed
		{
			free(buffer); // Clear buffer
			buffer = (char*)malloc(size - bufferSize); //Reallocate buffer for remains
			fread(buffer, 1, size - bufferSize, fs); // Read in remains
			hash.add(buffer, size - bufferSize); // Hash remains
		}
		hash_out = hash.getHash(); // Get final hash string
		fclose(fs); // Close file stream
		free(buffer); 
	}
	the_clock::time_point end = the_clock::now(); // End timer
	time_taken = duration_cast<chrono::microseconds>(end - start).count(); // Get time taken as int*/
	farmRef->AddTask(filePath, size, time_taken, hash_out); // Send data to be written to disk
	return;
	}

WriteTask::WriteTask(string a, uint64_t b, int c, string d)
	{
		filePath = a;
		size = b;
		time_taken = c;
		hash_out = d;
	}
void WriteTask::run() {
		
	{
		unique_lock<mutex> lock(diskWriterProtect);
		fstream outfile; // Initialise filestream
		outfile.open("files.txt", fstream::out | fstream::app); // Open files.txt for writing at end of file
		outfile << filePath << "\nSize : " << size << " bytes\n" << "Time taken : " << time_taken << "us\n" << hash_out << "\n\n"; // Output file path and hash to results file
		outfile.close(); // End filestream
	}
		return;
	}
