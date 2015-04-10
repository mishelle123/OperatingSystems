/*
 * pft.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: mishelle
 */

#include "pft.h"
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>
const std::string PFT_INIT_ERROR =  "pft_init error: ";
const std::string PFT_DONE_ERROR =  "pft_done error: ";
const std::string PFT_SET_PARAL_ERROR =  "setParallelismLevel error: ";
const std::string PFT_FIND_TYPES_ERROR =  "pft_find_types error: ";
const std::string PFT_GET_STATS_ERROR =  "pft_get_stats error: ";



typedef struct childStruct{
	int readFromChild;
	int writeToChild;
	int start, end;
	int iti;
	bool readyToWrite;
	std::string remainingBuffer;

}childStruct;

std::vector<childStruct> children;
int parallelismLevel;
const int CHUNK_SIZE = 50;
pft_stats_struct stats;

std::string lastError = "";

/*
Initialize the pft library.
Argument:
	"n" is the level of parallelism to use (number of parallel ‘file’) commands.
This method should initialize the library with empty statistics.

A failure may happen if a system call fails (e.g. alloc) or if n is not positive.
Return value:
	A valid error message, started with "pft_init error:" should be obtained by using the pft_get_error().
 */
int pft_init(int n)
{
	/**
	 * validates that the number is possitive
	 */
	if(n < 0)
	{
		lastError = PFT_INIT_ERROR + "'n' must be possitive";
		return FAILURE;
	}

	parallelismLevel = n;
	stats.file_num = 0;
	stats.time_sec = 0;

	for(int i = 0; i < n ; i++ )
	{


		int fdRead[2], fdWrite[2];
		pid_t pid;

		// creates pipes and check if sucesseded
		if ( (pipe(fdRead) < 0) || (pipe(fdWrite) < 0) )
		{
			lastError = PFT_INIT_ERROR + "SYSTEM ERROR - pipe() failed";
			return FAILURE;
		}
		fd_set readfds;
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 1;
		pid = fork();
		// fork failed
		if(pid < 0)
		{
			lastError = PFT_INIT_ERROR + "SYSTEM ERROR - fork() failed";
			return FAILURE;
		}

		// we are in the child process
		else if(pid == 0)
		{
			////////////////////check//////////////////////////
			if(dup2(fdWrite[0], STDIN_FILENO) < 0 || dup2(fdRead[1], STDOUT_FILENO) < 0)
			{
				_exit(-1);
			}

			if((close(fdWrite[1]) < 0) || close(fdRead[0]) < 0 ||
				(close(fdWrite[0]) < 0) || close(fdRead[1]) < 0)
			{
				_exit(-1);
			}
			// calls execl
			if (execl("/usr/bin/file", "file", "-n","-f-",NULL) < 0 )
			{
				_exit(-1);
			}
			_exit(-1);
		}

		// we are in the parent process
		else
		{
			if((close(fdWrite[0]) < 0) || close(fdRead[1]) < 0)
			{
				lastError = PFT_INIT_ERROR + "SYSTEM ERROR - close() failed";
				return FAILURE;
			}

			//check if the child process didn't fail
			FD_ZERO(&readfds);
			FD_SET(fdRead[0],&readfds);
			if(select(fdRead[0] + 1, &readfds, NULL,NULL,&tv) < 0)
			{
				lastError = PFT_INIT_ERROR + "SYSTEM ERROR - select() failed";
				return FAILURE;
			}
			if(FD_ISSET(fdRead[0],&readfds))
			{
				lastError = PFT_INIT_ERROR + "something went wrong in the child process ";
				return FAILURE;
			}

			//save the child process info
			childStruct child;
			child.readFromChild = fdRead[0];
			child.writeToChild = fdWrite[1];
			children.push_back(child);
		}

	}
	return SUCCESS;

}


/*
This function called when the user finished to use the library.
Its purpose is to free the memory/resources that allocated by the library.

The function return error FAILURE if a system call fails.
Return value:
	On success return SUCCESS, on error return FAILURE.
	A valid error message, started with "pft_done error:" should be obtained by using the pft_get_error().
 */
int pft_done()
{
	// we close all the fd's and empty the children vector
	for(int i = parallelismLevel - 1; i >= 0 ; i-- )
	{
		if((close(children[i].readFromChild) < 0) || close(children[i].writeToChild) < 0)
		{
			lastError = PFT_DONE_ERROR + "SYSTEM ERROR - close() failed";
			return FAILURE;
		}
		children.pop_back();
	}

	return SUCCESS;
}

/*
Set the parallelism level.
Argument:
	"n" is the level of parallelism to use (number of parallel ‘file’) commands.
A failure may happen if a system call fails (e.g. alloc) or n is not positive.
Return value:
	On success return SUCCESS, on error return FAILURE.
	A valid error message, started with "setParallelismLevel error:" should be obtained by using the pft_get_error().
 */
int setParallelismLevel(int n)
{
	// check if there is no need to change the level
	if(n == parallelismLevel || n < 1 )
	{
		pft_clear_stats();
		return SUCCESS;
	}
	// reset the old parallism level
	if(pft_done() < 0)
	{
		lastError = PFT_SET_PARAL_ERROR + "setParallelismLevel() failed";
		return FAILURE;
	}
	// initialize the parallelism level
	if(pft_init(n) < 0)
	{
		lastError = PFT_SET_PARAL_ERROR + "setParallelismLevel() failed";
		return FAILURE;
	}
	return SUCCESS;
}


// Return the last error message.
// The message should be empty if there was no error since the last initialization.
// This function must not fail.
const std::string pft_get_error()
{
	return lastError;
}


/**
 * return true iff all the processes are done
 */
bool isFinished()
{

	for(int i = 0; i < parallelismLevel ; i++)
	{

		if(children[i].end >= children[i].iti)
		{
			return false;
		}

	}
	return true;

}

/**
 * return the maximum fd in use
 */
int findMaxfd()
{
	int maxfd = 0;
	for(int i = 0; i < parallelismLevel; i++)
	{
		maxfd = (children[i].readFromChild > maxfd) ? children[i].readFromChild : maxfd;
		maxfd = (children[i].writeToChild > maxfd) ? children[i].writeToChild : maxfd;
	}
	return maxfd;
}

/**
 * returns string with all the names of the files
 */
std:: string strAppend(std::vector<std::string>& file_names_vec, int start, int end)
{
	std:: string s = "";
	for(int i = start; i <= end ; i++)
	{
		s.append(file_names_vec[i]);
		s.append("\n");
	}
	return s;
}

/**
 * initialize the indices and decides how to allocate the files
 */
void initializeIndices(int namesVecSize, std::vector<std::string>& types_vec)
{

	if(namesVecSize < parallelismLevel)
	{
		setParallelismLevel(namesVecSize);
	}
	// validates the vector size
	int differ = namesVecSize - types_vec.size();

	if(differ > 0)
	{
		for(int i = 0; i < differ; i++)
		{
			types_vec.push_back("");
		}
	}

	if(namesVecSize < parallelismLevel * CHUNK_SIZE)
	{
		int averageChunkSize = namesVecSize / parallelismLevel;

		for(int i = 0; i < parallelismLevel ; i++)
		{
			children[i].start = children[i].iti = i * averageChunkSize ;
			children[i].end = ((i + 1) * averageChunkSize) - 1;
			children[i].readyToWrite = true;

		}

		// add the last indexes to the last child
		int reminder = namesVecSize - (parallelismLevel * averageChunkSize);
		if(reminder > 0)
		{
			children[parallelismLevel - 1].end += reminder;

		}
	}
	// file_names_vec.size() >= parallelismLevel * CHUNK_SIZE
	else
	{

		for(int i = 0; i < parallelismLevel ; i++)
		{
			children[i].start = children[i].iti = i * CHUNK_SIZE ;
			children[i].end = ((i + 1) * CHUNK_SIZE) - 1;
			children[i].readyToWrite = true;

		}
	}
}

/**
 * perform the parsing of the buffer
 */
void parseBuffer(std::vector<std::string>& types_vec, std::string buffer, int child)
{
	std::size_t pos = buffer.find_first_of('\n');
	std::string halfLine = "";
	if(pos!=std::string::npos)
	{
		//Separate the given string to lines and put each line in the type vector
		std::string line = children[child].remainingBuffer.append(buffer.substr(0,pos));
		while (pos != std::string::npos)
		{
			types_vec[children[child].iti++] = line;
			stats.file_num++;
			buffer = buffer.substr(pos + 1);
			pos = buffer.find_first_of('\n');
			line = buffer.substr(0, pos);
		}
		children[child].remainingBuffer = buffer;
	}

}

/*
This function uses ‘file’ to calculparallelismLevelate the type of each file in the given vector using n parallelism level.
It gets a vector contains the name of the files to check (file_names_vec) and an empty vector (types_vec).
The function runs "file" command on each file in the file_names_vec (even if it is not a valid file) using n parallelism level,
and insert its result to the same index in types_vec.

The function fails if any of his parameters is null, if types_vec is not an empty vector or if a system called failed
(for example fork failed).

Parameters:
	file_names_vec - a vector contains the absolute or relative paths of the files to check.
	types_vec - an empty vector that will be initialized with the results of "file" command on each file in file_names_vec.
Return value:
	On success return parallelismLevelSUCCESS, on error return FAILURE.
	A valid error message, started with "pft_find_types error:" should be obtained by using the pft_get_error().
 */
int pft_find_types(std::vector<std::string>& file_names_vec, std::vector<std::string>& types_vec)
{
	struct timeval start, end;

	initializeIndices(file_names_vec.size(), types_vec);

	if(gettimeofday(&start, NULL) == -1)
	{
		lastError = PFT_FIND_TYPES_ERROR + "SYSTEM ERROR - gettimeofday() failed";
		return FAILURE;
	}

	unsigned int currentEnd = children[parallelismLevel - 1].end;

	fd_set readfds, writefds;
	int maxfd = findMaxfd();


	// read and write the files names
	while(currentEnd != (file_names_vec.size() - 1) || !isFinished())
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		// adds the fd to the set
		for(int i = 0; i < parallelismLevel ; i++)
		{
			FD_SET(children[i].readFromChild, &readfds);
			FD_SET(children[i].writeToChild, &writefds);
		}


		if(select(maxfd + 1, &readfds, &writefds,  NULL, NULL) < 0)
		{
			lastError = PFT_FIND_TYPES_ERROR + "SYSTEM ERROR - select() failed";
			return FAILURE;
		}

		for(int i = 0; i < parallelismLevel ; i++)
		{
			if(FD_ISSET(children[i].writeToChild, &writefds) && children[i].readyToWrite)
			{
				std::string filesNames = strAppend(file_names_vec,
						children[i].start, children[i].end);

				if(write(children[i].writeToChild, filesNames.c_str(),
						filesNames.length()) < 0)
				{
					lastError = PFT_FIND_TYPES_ERROR + "SYSTEM ERROR - write() failed";
					return FAILURE;
				}
				children[i].readyToWrite = false;
			}

		}

		for(int i = 0; i < parallelismLevel ; i++)
		{
			if(FD_ISSET(children[i].readFromChild, &readfds))
			{
				char buffer[PIPE_BUF] = {0};
				buffer[PIPE_BUF - 1] = '\0';
				if(read(children[i].readFromChild, buffer, PIPE_BUF - 1) < 0)
				{
					lastError = PFT_FIND_TYPES_ERROR + "SYSTEM ERROR - read() failed";
					return FAILURE;
				}
				std::string s(buffer);
				parseBuffer(types_vec, s, i);
				if((children[i].iti > children[i].end) &&
						(currentEnd != (file_names_vec.size() - 1)))
				{

					//there are files left- the number of files is bigger than chunksize
					if(currentEnd + CHUNK_SIZE < file_names_vec.size() - 1)
					{
						children[i].readyToWrite = true;
						children[i].start = children[i].iti = currentEnd + 1;
						children[i].end = currentEnd + CHUNK_SIZE;
					}
					//there are files left- the number of files is smaller than chunksize
					else
					{
						children[i].readyToWrite = true;
						children[i].start = children[i].iti = currentEnd + 1;
						children[i].end = file_names_vec.size() - 1;
					}
					currentEnd = children[i].end;


				}

			}

		}

	}


	if(gettimeofday(&end, NULL) == -1)
	{
		lastError = PFT_FIND_TYPES_ERROR + "SYSTEM ERROR - gettimeofday() failed";
		return FAILURE;
	}

	//stats.file_num += file_names_vec.size();
	stats.time_sec += ((end.tv_sec  + end.tv_usec/(double)1000000) -
			(start.tv_sec  + start.tv_usec/(double)1000000));
	return SUCCESS;

}

/*
This method initialize the given pft_stats_struct with the current statistics collected by the pft library.

If statistic is not null, the function assumes that the given structure have been allocated,
and updates file_num to contain the total number of files processed up to now
and time_sec to contain the total time in seconds spent in processing.

A failure may happen if a system call fails or if the statistic is null.

For example, if I call "pft_init", wait 3 seconds, call "pft_find_types" that read 7,000,000 files in 5 seconds, and then call to this
method, the statistics should be: statistic->time_sec = 5 and statistic->file_num = 7,000,000.

Parameter:
	statistic - a pft_stats_struct structure that will be initialized with the current statistics.
Return value:
	On success return SUCCESS, on error return FAILURE.
	A valid error message, started with "pft_get_stats error:" should be obtained by using the pft_get_error().
 */
int pft_get_stats(pft_stats_struct *statistic)
{
	if(statistic == NULL)
	{
		lastError = PFT_GET_STATS_ERROR + "'*statistic' is NULL";
		return FAILURE;
	}

	statistic->file_num = stats.file_num;
	statistic->time_sec = stats.time_sec;
	return SUCCESS;
}


// Clear the statistics setting all to 0.
// The function must not fail.
void pft_clear_stats()
{
	stats.file_num = 0;
	stats.time_sec = 0;
}

