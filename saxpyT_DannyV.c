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
 //To compile gcc -Wall -o saxpyT_DannyV saxpyT_DannyV -lpthread
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

void* compute(void *);

typedef struct _param{
	int ini;
	int end;
	double* X;
	double a;
	double* Y;
	double* Y_avgs;
	int it;
	int p;

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
	int i, it;
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

	printf("a= %f \n", a);	
#endif

	/*
	 *	Function to parallelize 
	 */
	gettimeofday(&t_start, NULL);

	pthread_t th1;
	pthread_t th2;

	param_t param1;
	param_t param2;

	param1.ini = 0;
	param1.end = p/2;
	param2.ini = p/2;
	param2.end = p;

	param1.X = X;
	param1.a = a;
	param1.Y = Y;
	param1.Y_avgs = Y_avgs;

	param2.X = X;
	param2.a = a;
	param2.Y = Y;
	param2.Y_avgs = Y_avgs;

	param1.p = p;
	param2.p = p;

	sem_init(&mutex, 0 ,1);

	//SAXPY iterative SAXPY mfunction
	for(it = 0; it < max_iters; it++){
		//crear hilo
		param1.it = it;
		param2.it = it;
		pthread_create(&th1, NULL, &compute, &param1);
		pthread_create(&th2, NULL, &compute, &param2);

		//join hilos
		pthread_join(th1,NULL);
		pthread_join(th2,NULL);
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
	param_t* par = (param_t *) arg;

	int ini = par->ini;
	int end = par->end;

	double* X = par->X;
	double a = par->a;
	double* Y = par->Y;
	double* Y_avgs = par->Y_avgs;
	int it = par->it;
	int i;
	double acc = 0;

	int p = par->p;

	for(i = ini; i < end; i++){
		Y[i] = Y[i] + a * X[i];
		//Y_avgs[it] += Y[i];
		acc += Y[i];		
	}

	sem_wait(&mutex);
	Y_avgs[it] = acc / p;
	sem_post(&mutex);	 

	return NULL;
}