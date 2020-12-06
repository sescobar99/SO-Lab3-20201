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
#include <semaphore.h>


void* compute (void *);

typedef struct _param{
	int ini;
	int end;
	double* X;
	double a;
	double* Y;
	double* Y_avgs;
	int it;
	int p;
	int max_iters;
} param_t;

sem_t mutex;

int main(int argc, char* argv[]){
	// Variables to obtain command line parameters
	unsigned int seed = 1;
  	int p = 10000000;
  	int n_threads = 2;
  	int max_iters = 1000;
  	// Variables to perform SAXPY operation
	double* X;
	double a;
	double* Y;
	double* Y_avgs;
	int i;
	// Variables to get execution time
	struct timeval t_start, t_end;
	double exec_time;

	// Getting input values
	int opt;
	while((opt = getopt(argc, argv, ":p:s:n:i:")) != -1){
		switch(opt){
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

	printf("p = %d, seed = %d, n_threads = %d, max_iters = %d\n", \
	 p, seed, n_threads, max_iters);

	// initializing data
	X = (double*) malloc(sizeof(double) * p);
	Y = (double*) malloc(sizeof(double) * p);
	Y_avgs = (double*) malloc(sizeof(double) * max_iters);

	for(i = 0; i < p; i++){
		X[i] = (double)rand() / RAND_MAX;
		Y[i] = (double)rand() / RAND_MAX;
	}
	for(i = 0; i < max_iters; i++){
		Y_avgs[i] = 0.0;
	}
	a = (double)rand() / RAND_MAX;

#ifdef DEBUG
	printf("vector X= [ ");
	for(i = 0; i < p-1; i++){
		printf("%f, ",X[i]);
	}
	printf("%f ]\n",X[p-1]);

	printf("vector Y= [ ");
	for(i = 0; i < p-1; i++){
		printf("%f, ", Y[i]);
	}
	printf("%f ]\n", Y[p-1]);

	printf("a= %f \n", a)
#endif

	/*
	 *	Function to parallelize
	 */
	gettimeofday(&t_start, NULL);

	pthread_t threads[n_threads];
	param_t params[n_threads];

	int rc;
	int t;
	void *status;
	

	sem_init(&mutex,0,1);
	//Create threads
	for (t = 0; t < n_threads; t++)
	{
		//pasar valores para operar los vectores
		params[t].ini = (p / n_threads) * t;
		params[t].end = (p / n_threads) * (t + 1);
		if(t == n_threads-1){
			params[t].end = p;
		}
		params[t].X = X;
		params[t].Y = Y;
		params[t].a = a;
		params[t].Y_avgs = Y_avgs;
		params[t].p = p;
		params[t].max_iters = max_iters;

		#ifdef DEBUG
		printf("In main: creating thread %d\n", t);
		#endif
		rc = pthread_create(&threads[t], NULL, &compute, &params[t]);
		
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
	
	gettimeofday(&t_end, NULL);

#ifdef DEBUG
	printf("RES: final vector Y= [ ");
	for(i = 0; i < p-1; i++){
		printf("%f, ", Y[i]);
	}
	printf("%f ]\n", Y[p-1]);
#endif

	// Computing execution time
	exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;  // sec to ms
	exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
	printf("Execution time: %f ms \n", exec_time);
	printf("Last 3 values of Y: %f, %f, %f \n", Y[p-3], Y[p-2], Y[p-1]);
	printf("Last 3 values of Y_avgs: %f, %f, %f \n", Y_avgs[max_iters-3], Y_avgs[max_iters-2], Y_avgs[max_iters-1]);
	return 0;
}

void* compute (void *arg){
	param_t* par =  (param_t *) arg;
	int ini = par->ini;
	int end = par->end;
	double* X = par->X;
	double a = par->a;
	double* Y = par->Y;
	double* Y_avgs = par->Y_avgs;
	int it = par->it;
	int p = par->p;
	int i;
	int max_iters = par->max_iters;
	double acc;
	#ifdef DEBUG
	printf("Thread values start = %d, end = %d, max_iters = %d, p = %d \n", ini, end, max_iters, p);
	#endif
	//SAXPY iterative SAXPY mfunction
	for(it = 0; it < max_iters; it++){
		acc = 0;
		for(i = ini; i < end; i++){
			Y[i] = Y[i] + a * X[i];
			acc += Y[i];
		}
		sem_wait(&mutex);
		Y_avgs[it] += acc/p;
		sem_post(&mutex);
	}

	return NULL;
}
