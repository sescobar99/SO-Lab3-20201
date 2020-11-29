/**
 * @defgroup   SAXPY saxpy
 *
 * @brief      This file implements an iterative saxpy operation
 * 
 * @param[in] <-p> {vector size} 
 * @param[in] <-s> {seed}
 * @param[in] <-n> {number of threads to create} 
 * @param[in] <-i> {maximum itertions} 
 *
 * @author     Danny Munera
 * @date       2020
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <pthread.h>

/* Define globally accessible variables to perform SAXPY operation and a mutex */
double *X;
double a;
double *Y;
double *Y_avgs;
pthread_mutex_t mutex;

typedef struct
{
	int start;
	int end;
	int max_iters;
} thread_args;

void *thread_SAXPY(void *args)
{
	thread_args *input = args;	
	int end = input->end;
	int max_iters = input->max_iters;
	printf("Thread values start = %d, end = %d, max_iters = %d \n", input->start, end, max_iters);
	//SAXPY iterative SAXPY mfunction
	for (int it = 0; it < max_iters; it++)
	{
		for (int i = input->start; i < end; i++)
		{
			Y[i] = Y[i] + a * X[i];
			//pthread_mutex_lock(&mutex);
			Y_avgs[it] += Y[i]; //Critical section?
			//pthread_mutex_unlock(&mutex);
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	// Variables to obtain command line parameters
	unsigned int seed = 1;
	int p = 10000000;
	int n_threads = 2;
	int max_iters = 1000;
	// Variables to perform SAXPY operation
	int i, it;
	// Variables to get execution time
	struct timeval t_start, t_end;
	double exec_time;

	// Getting input values
	int opt;
	while ((opt = getopt(argc, argv, ":p:s:n:i:")) != -1)
	{
		switch (opt)
		{
		case 'p':
			printf("vector size: %s\n", optarg);
			p = strtol(optarg, NULL, 10);
			assert(p > 0 && p <= 2147483647);
			break;
		case 's':
			printf("seed: %s\n", optarg);
			seed = strtol(optarg, NULL, 10);
			break;
		case 'n':
			printf("threads number: %s\n", optarg);
			n_threads = strtol(optarg, NULL, 10);
			break;
		case 'i':
			printf("max. iterations: %s\n", optarg);
			max_iters = strtol(optarg, NULL, 10);
			break;
		case ':':
			printf("option -%c needs a value\n", optopt);
			break;
		case '?':
			fprintf(stderr, "Usage: %s [-p <vector size>] [-s <seed>] [-n <threads number>]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	srand(seed);

	printf("p = %d, seed = %d, n_threads = %d, max_iters = %d\n",
		   p, seed, n_threads, max_iters);

	// initializing data
	X = (double *)malloc(sizeof(double) * p);
	Y = (double *)malloc(sizeof(double) * p);
	Y_avgs = (double *)malloc(sizeof(double) * max_iters);

	for (i = 0; i < p; i++)
	{
		X[i] = (double)rand() / RAND_MAX;
		Y[i] = (double)rand() / RAND_MAX;
	}
	for (i = 0; i < max_iters; i++)
	{
		Y_avgs[i] = 0.0;
	}
	a = (double)rand() / RAND_MAX;

#ifdef DEBUG
	printf("vector X= [ ");
	for (i = 0; i < p - 1; i++)
	{
		printf("%f, ", X[i]);
	}
	printf("%f ]\n", X[p - 1]);

	printf("vector Y= [ ");
	for (i = 0; i < p - 1; i++)
	{
		printf("%f, ", Y[i]);
	}
	printf("%f ]\n", Y[p - 1]);

	printf("a= %f \n", a);
#endif

	/*
	 *	Function to parallelize 
	 */
	gettimeofday(&t_start, NULL);

	pthread_t threads[n_threads];
	thread_args thread_input_array[n_threads];
    //pthread_mutex_init(&mutex, NULL);
	int rc;
	int t;
	void *status;
	
	//Create threads
	for (t = 0; t < n_threads; t++)
	{
		thread_input_array[t].max_iters = max_iters;
		thread_input_array[t].start = (p / n_threads) * t;
		thread_input_array[t].end = (p / n_threads) * (t + 1);
		printf("In main: creating thread %d\n", t);
		rc = pthread_create(&threads[t], NULL, thread_SAXPY, (void *)&thread_input_array[t]);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	//Wait for threads
	for (t = 0; t < n_threads; t++)
	{
		rc = pthread_join(threads[t], &status);
		if (rc)
		{
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
#ifdef DEBUG
		printf("Main: completed join with thread %d having a status of %ld\n", t, (long)status);
#endif
	}
	 //pthread_mutex_destroy(&mutex);

	//Calculate avgs
	for (it = 0; it < max_iters; it++)
	{
		Y_avgs[it] = Y_avgs[it] / p;
	}

	gettimeofday(&t_end, NULL);

#ifdef DEBUG
	printf("RES: final vector Y= [ ");
	for (i = 0; i < p - 1; i++)
	{
		printf("%f, ", Y[i]);
	}
	printf("%f ]\n", Y[p - 1]);
#endif

	// Computing execution time
	exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;	 // sec to ms
	exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
	printf("Execution time: %f ms \n", exec_time);
	printf("Last 3 values of Y: %f, %f, %f \n", Y[p - 3], Y[p - 2], Y[p - 1]);
	printf("Last 3 values of Y_avgs: %f, %f, %f \n", Y_avgs[max_iters - 3], Y_avgs[max_iters - 2], Y_avgs[max_iters - 1]);
	return 0;
}