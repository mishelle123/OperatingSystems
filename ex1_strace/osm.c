/*
 * osm.c
 *
 *  Created on: Feb 25, 2014
 *      Author: mishelle
 */

#include "osm.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>

int osm_init();
double osm_function_time();
void foo();
char g_machineName[HOST_NAME_MAX];

#define DEFAULT_NUM_OF_ITI 50000;
#define NANOSEC_CALCULATOR(es, em, ss, sm) 1000*((es * 1000000 + em) - \
		(ss * 1000000 + sm));

/* Initialization function that the user must call
 * before running any other library function.
 * Returns 0 uppon success and -1 on failure
 */
int osm_init()
{
	// initialize the machine name
	return gethostname(g_machineName, HOST_NAME_MAX);
}

/* Time measurement function for an empty function call.
   returns time in nano-seconds upon success,
   and -1 upon failure.
 */
double osm_function_time(unsigned int osm_iterations)
{
	unsigned int numOfIterations = osm_iterations;
	// sets default value if the iterations number is 0
	if(osm_iterations == 0)
	{
		numOfIterations = DEFAULT_NUM_OF_ITI;
	}

	struct timeval start, end;
	// get the current time before the loop
	if(gettimeofday(&start, NULL) == -1)
	{
		// failure
		return -1;
	}
	unsigned int i;
	for(i = 0; i < numOfIterations ; i+=250)
	{
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();
		foo();



	}
	// get the current time after the loop
	if(gettimeofday(&end, NULL) == -1)
	{
		//failure
		return -1;
	}
	// convert the measured time to nanosecond
	double timeInNs = NANOSEC_CALCULATOR(end.tv_sec, end.tv_usec, start.tv_sec, start.tv_usec);
	return timeInNs / (double)numOfIterations;
}

/* Time measurement function for an empty trap into the operating system.
   returns time in nano-seconds upon success,
   and -1 upon failure.
 */
double osm_syscall_time(unsigned int osm_iterations)
{
	unsigned int numOfIterations = osm_iterations;
	// sets default value if the iterations number is 0
	if(osm_iterations == 0)
	{
		numOfIterations = DEFAULT_NUM_OF_ITI;
	}

	struct timeval start, end;
	// get the current time before the loop
	if(gettimeofday(&start, NULL) == -1)
	{
		// failure
		return -1;
	}
	unsigned int i;
	for(i = 0; i < numOfIterations ; i+=250)
	{
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;



	}
	// get the current time after the loop
	if(gettimeofday(&end, NULL) == -1)
	{
		//failure
		return -1;
	}
	// convert the measured time to nanosecond
	double timeInNs = NANOSEC_CALCULATOR(end.tv_sec, end.tv_usec, start.tv_sec, start.tv_usec);

	return timeInNs / (double)numOfIterations;
}

/* Time measurement function for a simple arithmetic operation.
   returns time in nano-seconds upon success,
   and -1 upon failure.
 */
double osm_operation_time(unsigned int osm_iterations)
{
	unsigned int numOfIterations = osm_iterations;
	// sets default value if the iterations number is 0
	if(osm_iterations == 0)
	{
		numOfIterations = DEFAULT_NUM_OF_ITI;
	}

	struct timeval start, end;
	// get the current time before the loop
	if(gettimeofday(&start, NULL) == -1)
	{
		// failure
		return -1;
	}

	unsigned int addition = 0;
	unsigned int i;
	for(i = 0; i < numOfIterations ; i+=250)
	{

		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;
		addition = i + 1;

	}
	// get the current time after the loop
	if(gettimeofday(&end, NULL) == -1)
	{
		//failure
		return -1;
	}
	if(addition < 0)
	{
		return -1;
	}
	// convert the measured time to nanosecond
	double timeInNs = NANOSEC_CALCULATOR(end.tv_sec, end.tv_usec, start.tv_sec, start.tv_usec);
	//
	return timeInNs / (double)(numOfIterations);
}

/**
 * return a timeMeasurmentStructure struct and initialize all its fields:
 * machineName - the name of the host,
 * numberOfIterations,
 * instructionTimeNanoSecond - the time takes to run a simple instruction
 * functionTimeNanoSecond - the time takes to call an empty function
 * trapTimeNanoSecond - the time takes to cahnge the os from user mode to kernel mode
 * functionInstructionRatio = functionTime / instructionTime,
 * trapInstructionRatio = trapTime / instructionTime
 */
timeMeasurmentStructure measureTimes (unsigned int osm_iterations)
{
	unsigned int numOfIterations = osm_iterations;
	// sets default value if the iterations number is 0
	if(osm_iterations == 0)
	{
		numOfIterations = DEFAULT_NUM_OF_ITI;
	}

	timeMeasurmentStructure toReturn;

	// initialize the struct
	toReturn.machineName = g_machineName;
	toReturn.numberOfIterations = numOfIterations;
	double operationTime = osm_operation_time(numOfIterations);
	double functionTime = osm_function_time(numOfIterations);
	double trapTime = osm_syscall_time(numOfIterations);
	toReturn.instructionTimeNanoSecond = operationTime;
	toReturn.functionTimeNanoSecond = functionTime;
	toReturn.trapTimeNanoSecond = trapTime;
	toReturn.functionInstructionRatio = functionTime / operationTime;
	toReturn.trapInstructionRatio = trapTime / operationTime;


	return toReturn;
}

void foo()
{
}


