/*
 * outputdevice1.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: mishelle
 */




#include "outputdevice.h"
#include <sstream>
#include <iostream>
#include <queue>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <limits.h>

enum
{
	SUCSSES = 0,
	SYSTEM_ERROR = -2,
	FAILURE = -1,
};

pthread_mutex_t queueMutex;
pthread_mutex_t taskIDMutex;
pthread_mutex_t highestIDMutex;
pthread_t theThread;


FILE* pFile;

bool isClosed = true;
bool readyToClose = true;

unsigned int taskCounter = 0;

struct Task
{
	char* taskBuffer;
	int length;
	int taskId;
};

std::string sysErr = "system error\n";
std::queue<Task> theQueue;
std::vector<bool> taskIdVec;
int highestId = 0;


/**
 * helper method
 *
 */
void* doWrite(void *arg)
{

	// run as long as there is something to write
	while(!isClosed)
	{

		if(pthread_mutex_lock(&queueMutex) != 0)
		{
			std::cerr << sysErr;
			exit(SYSTEM_ERROR);
		}
		if(!theQueue.empty())
		{
			// writing the first task in the queue
			Task t = theQueue.front();

			fwrite(t.taskBuffer, sizeof(char), t.length,  pFile);
			taskCounter++;
			taskIdVec.at(t.taskId) = true;
			theQueue.pop();
			delete t.taskBuffer;
		}
		else if(readyToClose)
		{
			isClosed = true;
		}

		if(pthread_mutex_unlock(&queueMutex) != 0)
		{
			std::cerr << sysErr;
			exit(SYSTEM_ERROR);
		}
	}

	//destroy unrelevant mutexes
	if(pthread_mutex_destroy(&queueMutex) != 0)
	{
		std::cerr << sysErr;
		exit(SYSTEM_ERROR);
	}

	if(pthread_mutex_destroy(&highestIDMutex) != 0)
	{
		std::cerr << sysErr;
		exit(SYSTEM_ERROR);
	}
	if(pthread_mutex_destroy(&taskIDMutex) != 0)
	{
		std::cerr << sysErr;
		exit(SYSTEM_ERROR);
	}
	fclose(pFile);
	return NULL;

}





/*
 * DESCRIPTION: The function creates the file filename if it does not exist and open the file for writing.
 *              This function should be called prior to any other functions as a necessary precondition for their success.
 *
 * RETURN VALUE: On success 0, -2 for a filesystem error (inability to create the file, etc.), otherwise -1.
 */
int initdevice(char *filename)
{

	if(!isClosed && !readyToClose)
	{
		return FAILURE;
	}
	taskCounter = 0;
	// initialize the boolean variables
	isClosed = false;
	readyToClose = false;

	pFile = fopen(filename, "a");
	// check for system error
	if(pFile == NULL)
	{
		return SYSTEM_ERROR;
	}

	if(pthread_mutex_init(&queueMutex, NULL) != 0 ||
			pthread_mutex_init(&taskIDMutex, NULL) != 0 ||
			pthread_mutex_init(&highestIDMutex, NULL) != 0)
	{
		return SYSTEM_ERROR;
	}
	//creates threads and checks if the system call failed
	if(pthread_create(&theThread, NULL, doWrite, NULL) != 0)
	{
		return SYSTEM_ERROR;
	}



	return SUCSSES;

}

/**
 * finds the lowest free id
 * return the lowest task id
 */
int searchForLowestId()
{
	int vecLength = taskIdVec.size();

	for(int i = 0 ; i < vecLength ; i++)
	{
		// reuse the task ID
		if(taskIdVec.at(i) == true)
		{
			return i;
		}
	}
	return vecLength;
}


/*
 * DESCRIPTION: The function writes the input buffer to the file. The buffer may be freed once this call returns.
 *              You should deal with any memory management issues.
 * 		Note this is non-blocking package you are required to implement you should return ASAP,
 * 		even if the buffer has not yet been written to the disk.
 *
 * RETURN VALUE: On success, the function returns a task_id (>= 0), which identifies this write operation.
 * 		 Note, you should reuse task_ids when they become available.  On failure, -1 will be returned.
 */
int write2device(char *buffer, int length)
{
	if(!readyToClose && !isClosed)
	{
		Task newTask;

		// generate the task id
		if(pthread_mutex_lock(&taskIDMutex) != 0)
		{
			return SYSTEM_ERROR;
		}
		taskIdVec.push_back(true);
		newTask.taskId = searchForLowestId();
		taskIdVec.at(newTask.taskId) = false;

		if(pthread_mutex_unlock(&taskIDMutex) != 0)
		{
			return SYSTEM_ERROR;
		}

		// check the highest Id
		if(pthread_mutex_lock(&highestIDMutex) != 0)
		{
			return SYSTEM_ERROR;
		}

		if(highestId < newTask.taskId)
		{
			highestId = newTask.taskId;
		}
		if(pthread_mutex_unlock(&highestIDMutex) != 0)
		{
			return SYSTEM_ERROR;
		}

		// copy the buffer to task
		char* copy = new char[length + 1];
		memcpy(copy, buffer, length);
		newTask.taskBuffer = copy;
		newTask.length = length;

		// add a task to the queue
		if(pthread_mutex_lock(&queueMutex) != 0)
		{
			return SYSTEM_ERROR;
		}
		theQueue.push(newTask);
		if(pthread_mutex_unlock(&queueMutex) != 0)
		{
			return SYSTEM_ERROR;
		}
		return newTask.taskId;
	}
	else
	{
		return FAILURE;
	}

}



/*
 * DESCRIPTION: Block until the specified task_id has been written to the file.
 * 		The task_id is a value that was previously returned by write2device function.
 * 		In case of task_id doesn't exist, should return -2; In case of other errors, return -1.
 *
 */
int flush2device(int task_id)
{
	if(isClosed)
	{
		return FAILURE;
	}

	//check if the id given is valid
	if(pthread_mutex_lock(&highestIDMutex) != 0)
	{
		return SYSTEM_ERROR;
	}

	if(task_id > highestId)
	{
		if(pthread_mutex_unlock(&highestIDMutex) != 0)
		{
			return SYSTEM_ERROR;
		}
		return SYSTEM_ERROR;
	}
	if(pthread_mutex_unlock(&highestIDMutex) != 0)
	{
		return SYSTEM_ERROR;
	}

	// busy wait
	while(wasItWritten(task_id) == 1){}
	return 1;
}

/*
 * DESCRIPTION: return (withoug blocking) how many tasks have been written to file since last initdevice.
 *      If number exceeds MAX_INT, return MIN_INT.
 * 		In case of error, return -1.
 *
 */
int howManyWritten()
{
	if(isClosed)
	{
		return FAILURE;
	}
	if(taskCounter > INT_MAX)
	{
		return INT_MIN;
	}
	return taskCounter;
}

/*
 * DESCRIPTION: return (withoug blocking) whether the specified task_id has been written to the file
 *      (0 if yes, 1 if not).
 * 		The task_id is a value that was previously returned by write2device function.
 * 		In case of task_id doesn't exist, should return -2; In case of other errors, return -1.
 *
 */
int wasItWritten(int task_id)
{
	if(isClosed)
	{
		return FAILURE;
	}

	//check if the id given is valid
	if(pthread_mutex_lock(&highestIDMutex) != 0)
	{
		return SYSTEM_ERROR;
	}
	if(task_id > highestId)
	{
		if(pthread_mutex_unlock(&highestIDMutex) != 0)
		{
			return SYSTEM_ERROR;
		}
		return SYSTEM_ERROR;
	}
	if(pthread_mutex_unlock(&highestIDMutex) != 0)
	{
		return SYSTEM_ERROR;
	}


	//check if the task was written
	if(pthread_mutex_lock(&taskIDMutex) != 0)
	{
		return SYSTEM_ERROR;
	}
	int toReturn = (taskIdVec.at(task_id) == true) ? 0 : 1;
	if(pthread_mutex_unlock(&taskIDMutex) != 0)
	{
		return SYSTEM_ERROR;
	}
	return toReturn;
}

/*
 * DESCRIPTION: close the output file and reset the system so that it is possible to call initdevice again.
 *              All pending task_ids should be written to output disk file.
 *              Any attempt to write new buffers (or initialize) while the system is shutting down should
 *              cause an error.
 *              In case of error, the function should cause the process to exit.
 *
 */
void closedevice()
{

	//prepare the device for closing
	readyToClose = true;
}

/*
 * DESCRIPTION: Wait for closedevice to finish.
 *              If closing was successful, it returns 1.
 *              If closedevice was not called it should return -2.
 *              In case of other error, it should return -1.
 *
 */
int wait4close()
{
	if(!readyToClose)
	{
		return SYSTEM_ERROR;
	}

	pthread_join(theThread, NULL);

	return 1;
}


