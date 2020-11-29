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

  	// Variables to perform SAXPY operation
	double* X;
	double a;
	double* Y;
	double* Y_avgs;

typedef struct {
    //Or whatever information that you need
    int start;
    int end;
	int max_iters;
} thread_args;

void * thread_SAXPY(void *args){
	thread_args *input = args;
	// int i = input->start;
	int end = input->end;
	int max_iters = input->max_iters;
	printf("i = %d, end = %d, max_iters = %d \n",input->start,end,max_iters);
	for(int it = 0; it < max_iters; it++){
		for(int i = input->start ; i < end ; i++){
				Y[i] = Y[i] + a * X[i];
				Y_avgs[it] += Y[i]; //Critical section
			}			
	}
	// free(input); i = 5.000.000, end = 10.000.000, max_iters = 1000 
    return 0;
}



int main(int argc, char* argv[]){
	// Variables to obtain command line parameters
	unsigned int seed = 1;
  	int p = 10000000;
  	int n_threads = 2;
  	int max_iters = 1000;
  	// Variables to perform SAXPY operation
	// double* X;
	// double a;
	// double* Y;
	// double* Y_avgs;
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
	//SAXPY iterative SAXPY mfunction

	pthread_t threads[n_threads];
    int rc;
	int t;
	// thread_args *thread_input = malloc(sizeof *thread_input);	
	// thread_input->max_iters	= max_iters;
    for(t = 0; t <n_threads; t++){
		thread_args *thread_input = malloc(sizeof *thread_input);	
		thread_input->max_iters	= max_iters;
		thread_input->start = (p/n_threads)*t;
		thread_input->end 	= (p/n_threads)*(t+1);
       	printf("In main: creating thread %d\n", t);
		rc = pthread_create(&threads[t], NULL, thread_SAXPY, thread_input);
		printf("Create Thread %d : i = %d, end = %d, max_iters = %d \n",rc ,thread_input->start,thread_input->end,thread_input->max_iters);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}

	}

	for(t = 0; t <n_threads; t++){
		pthread_join(threads[t], NULL);
	}

	for(it = 0; it < max_iters; it++){
		Y_avgs[it] = Y_avgs[it] / p;
	}

	//Create threads
	// int aux = 8;

	// thread_args *thread_input1 = malloc(sizeof *thread_input1);	
	// thread_input1->start = (p/aux)*0;
	// thread_input1->end 	= (p/aux)*1;
	// thread_input1->max_iters	= max_iters;

	// thread_args *thread_input2 = malloc(sizeof *thread_input2);
	// thread_input2->start = (p/aux)*1;
	// thread_input2->end 	= (p/aux)*2;
	// thread_input2->max_iters	= max_iters;

	// thread_args *thread_input3 = malloc(sizeof *thread_input3);
	// thread_input3->start = (p/aux)*2;
	// thread_input3->end 	= (p/aux)*3;
	// thread_input3->max_iters	= max_iters;

	// thread_args *thread_input4 = malloc(sizeof *thread_input4);
	// thread_input4->start = (p/aux)*3;
	// thread_input4->end 	= (p/aux)*4;
	// thread_input4->max_iters	= max_iters;

	// thread_args *thread_input5 = malloc(sizeof *thread_input5);
	// thread_input5->start = (p/aux)*4;
	// thread_input5->end 	= (p/aux)*5;
	// thread_input5->max_iters	= max_iters;

	// thread_args *thread_input6 = malloc(sizeof *thread_input6);
	// thread_input6->start = (p/aux)*5;
	// thread_input6->end 	= (p/aux)*6;
	// thread_input6->max_iters	= max_iters;

	// thread_args *thread_input7 = malloc(sizeof *thread_input7);
	// thread_input7->start = (p/aux)*6;
	// thread_input7->end 	= (p/aux)*7;
	// thread_input7->max_iters	= max_iters;

	// thread_args *thread_input8 = malloc(sizeof *thread_input8);
	// thread_input8->start = (p/aux)*7;
	// thread_input8->end 	= p;
	// thread_input8->max_iters	= max_iters;

	// pthread_t t1;
	// pthread_t t2;
	// pthread_t t3;
	// pthread_t t4;
	// pthread_t t5;
	// pthread_t t6;
	// pthread_t t7;
	// pthread_t t8;
	// pthread_create(&t1, NULL, thread_SAXPY,thread_input1);
	// pthread_create(&t2, NULL, thread_SAXPY,thread_input2);
	// pthread_create(&t3, NULL, thread_SAXPY,thread_input3);
	// pthread_create(&t4, NULL, thread_SAXPY,thread_input4);
	// pthread_create(&t5, NULL, thread_SAXPY,thread_input5);
	// pthread_create(&t6, NULL, thread_SAXPY,thread_input6);
	// pthread_create(&t7, NULL, thread_SAXPY,thread_input7);
	// pthread_create(&t8, NULL, thread_SAXPY,thread_input8);
	// pthread_join(t1, NULL);
	// pthread_join(t2, NULL);
	// pthread_join(t3, NULL);
	// pthread_join(t4, NULL);
	// pthread_join(t5, NULL);
	// pthread_join(t6, NULL);
	// pthread_join(t7, NULL);
	// pthread_join(t8, NULL);

	// for(it = 0; it < max_iters; it++){
		// for(i = 0; i < p; i++){
		// 	Y[i] = Y[i] + a * X[i];
		// 	Y_avgs[it] += Y[i];
		// }
		// Y_avgs[it] = Y_avgs[it] / p;
	// }
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