#include <fstream> // File I/O
#include <iostream> // Command line I/O
#include <string> // File paths
#include <thread> // Multithreading 
#include <chrono>
#include "Tasks.h" // Task classes 
//#include "MainClass.h" // Hash checking class

// Windows includes
#ifdef _WIN32
#include <experimental/filesystem> // Filesystem access
namespace fs = std::experimental::filesystem;

// Linux includes
#elif __linux__
#include <ftw.h>
queue<string> pathQueue;
queue<uint64_t> sizeQueue;
#endif

using namespace std;
using std::iostream;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
typedef std::chrono::steady_clock the_clock;

uint64_t totSize = 0; // Total bytes processed (Add each file size to this)




// Windows filesystem access
#ifdef _WIN32
void getFiles(string dir, ThreadPool* Farmer) {
	for (auto& p : fs::recursive_directory_iterator(dir)) { //For every filesystem entry in starting directory AND subdirectories
		if (!fs::is_directory(experimental::filesystem::status(p)) && fs::exists(p)) { // If entry is not a directory
			Farmer->AddTask(p.path().string(), fs::file_size(p), Farmer); //  Create a hash task for each file
			totSize = totSize + fs::file_size(p);
		}
	}
	return;
}

#elif __linux__
void getLFiles(const char *fpath, const struct stat *sb, int typeflag){
	if(typeflag == FTW_F){ // Check if file
		string filePath(fpath);
		
		if(sb->st_size!=0){
		pathQueue.push(filePath);
		sizeQueue.push(sb->st_size);		
		}
		totSize = totSize + sb->st_size;
	}
	return;	
}
#endif

int main(int argc, char* argv[])
{
	string dir;
	// User introduction
	cout << "---- Rapid File Hash ----\n\n\nPowerful SHA256 file hashing for digital forensics and malware detection.\nBuilt for high speed storage.\n\nWritten by Colin McKay Campbell. \nOpen source project can be found at github.com/ColinMcKayCampbell/RapidFileHash\n(Not public yet in this coursework build, check back after marking!)  \nAntivirus programs really don't like this.\nI suggest disabling real-time protection for the time being.\n\n";
	cout << "This program will use all available CPU power and drive I/O.\nThis can generate large amounts of heat and slow your system to a crawl.\nPlease ensure you have sufficient cooling and are not running any important servers others are relying on.\nPress Enter to continue.\n";
	getline(cin, dir);
	cout << "Starting directory: ";
	getline(cin, dir);

	ThreadPool CoreMuncher;
	cout << "\nInitialising...\n";
	fstream fs;
	fs.open("files.txt", fstream::trunc | fstream::out);
	fs << "Rapid File Hash by Colin McKay Campbell\n\n\n";
	fs.close();
		
		// Windows file acquisition
		#ifdef _WIN32		
		getFiles(dir, &CoreMuncher); // Get list of files to hash
		// Linux file acquisition
		#elif __linux__
		ftw(dir.c_str(),getLFiles,16);
		int queueSize = pathQueue.size();
		for(int x = 0; x < queueSize; x++){
			CoreMuncher.AddTask(pathQueue.front(),sizeQueue.front(),&CoreMuncher);
			pathQueue.pop();
			sizeQueue.pop();
		}
		CoreMuncher.SetReady(true);
		#endif
		the_clock::time_point start = the_clock::now(); // Start timer
		CoreMuncher.Execute();
		the_clock::time_point end = the_clock::now(); // End timer
		auto time_taken = duration_cast<chrono::microseconds>(end - start).count(); // Get time taken as int
	float readableSize = totSize / 1000000;
	if (readableSize > 0) {
		cout << readableSize << " megabytes processed in " << time_taken /1000 << " milliseconds.\n";
		fstream outfile; // Initialise filestream
		outfile.open("files.txt", fstream::out | fstream::app); // Open files.txt for writing at end of file
		outfile << readableSize << " megabytes process in " << time_taken / 1000 << " milliseconds.\n";
		outfile.close();
	}
	totSize = 0;
	return 0;
}
