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
	SHA256 hash;
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
	const char* path = filePath.c_str();
	fopen_s(filePass, path, "r");
	if (fs != NULL) {
		
		buffer = (char*)malloc(bufferSize);
		if (buffer == NULL) { fputs("Malloc error.\n", stderr); exit(2); }
		while (ftell(fs) < bufferSize) {
			fread(buffer, 1, bufferSize, fs);
			hash.add(buffer, bufferSize);
		}
		if(ftell(fs)!=size)
		{
			free(buffer);
			buffer = (char*)malloc(size - bufferSize);
			fread(buffer, 1, size - bufferSize, fs);
			hash.add(buffer, size - bufferSize);
		}
		hash_out = hash.getHash();
		fclose(fs);
		free(buffer);
	}
	the_clock::time_point end = the_clock::now(); // End timer
	
		/*const long int bufferSize = 128; // Bytes stored in RAM
		unsigned char fileBuffer[bufferSize]; // Array of bytes to read in to 
		const void* bytePoint[bufferSize]; // Point to fileBuffer for hashing function
		int count = 0; // Counter variable
		fstream fs; // Declare Filestream
		SHA256 hash; // Declare hash object
		filebuf* readIn = fs.rdbuf(); // Initialise readbuffer
		string hash_out = ""; // Output string
		long long time_taken = 0;
		
			the_clock::time_point start = the_clock::now(); // Start timer
			fs.open(filePath, fstream::binary | fstream::in); // Open file
			if (fs.is_open()) { // If file is open

				for (int progress = 0; progress < size; progress++) { // Repeat for length of file

					unsigned char byte = readIn->sbumpc(); // Get next byte (first time will take first byte)
					fileBuffer[count] = byte; // Add byte to byte array
					bytePoint[count] = &fileBuffer[count]; // Add to corresponding address array
					count++; // Increment counter

					if (count > bufferSize - 1) { // Once buffer is full
						hash.add(*bytePoint, bufferSize); // Process bytes in buffer
						count = 0; // Reset counter
					}

				} // End "progress loop"

				if (count != bufferSize) { // If there are bytes left in the fileBuffer
					hash.add(*bytePoint, count); // Process remains
					//count = 0; // Reset counter (Used for benchmarking)
				}

			}// End "If file is open"
			else {}
	
			hash_out = hash.getHash(); // Return hash as string
			fs.close(); // Close file read
			the_clock::time_point end = the_clock::now();
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
