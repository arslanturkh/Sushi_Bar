

#include <sstream>
#include <iostream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <cstdlib>
#include <semaphore.h>
#include <time.h>

using namespace std;
unsigned int microseconds;



// paramaters for the threads functions
struct threadFuncktionParameters {
	int threadCounter[1000000];
	
	
};

int myCounter = 0;
int eating = 0;
int waiting = 0;
bool mustWait = false;
int emptySeat = 5;


sem_t mutex;
sem_t block;

void *ThreadFunction(void* param);

int main(int argc, char **argv) {
	string customer = (string)argv[1];
	int numberOfCustomer = atoi(customer.c_str());
	int tid;
	threadFuncktionParameters paramater;// create struct
	sem_init(&mutex,0,1);
	sem_init(&block, 0, 4);
	srand(time(NULL));

	pthread_t threads[numberOfCustomer];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (int i = 0; i < numberOfCustomer; i++) {
		paramater.threadCounter[i] = i; // set thread counter to i
		// creating threads
		tid = pthread_create(&threads[i], &attr, ThreadFunction, (void*)&paramater);

		if (tid) {
			cout << "Creating thread error " << endl;
			exit(-1);
		}

	}

	pthread_attr_destroy(&attr);
	for (int i = 0; i < numberOfCustomer; i++) {
		tid = pthread_join(threads[i], 0); // threads wait for each other

		if (tid) {
			cout << "Error:unable to join, " << tid << endl;
			exit(-1);
		}
	}
}

void *ThreadFunction(void* param) {
	int waitingTime, eatingTime;
	struct threadFuncktionParameters* paramater;
	//struct sushiBar* paramater;
	paramater = (struct threadFuncktionParameters*) param; //get inputs
	string threadName;
	stringstream s;
	

	
	waitingTime = rand() % 1000001;
	usleep(waitingTime);

	s << (paramater->threadCounter[myCounter] + 1);
	threadName = "Thread " + s.str() + ": ";
	myCounter++;

	sem_wait(&mutex);

	if (mustWait) {
		cout << threadName << emptySeat << " seats are empty but I wait. there are other " << waiting+1 << " people waiting " << endl;
		waiting++;
		sem_post(&mutex);
		sem_wait(&block);
		waiting--;
	}

	if (emptySeat == 1) {
		cout << threadName << "I sit on the last seat. We are dining together now." << endl;
	}
	else {
		cout << threadName << emptySeat << " seats are empty so I sit." << endl;
	}

	eating++;
	emptySeat--;
	mustWait = (eating == 5);

	if ((waiting) && (!mustWait))
		sem_post(&block);
	else
		sem_post(&mutex);
	
	//eating	
	eatingTime = rand() % 901 + 100;
	usleep(eatingTime);

	sem_wait(&mutex);
	eating--;
	emptySeat++;


	if (eating == 0)
		mustWait = false;

	if ((waiting) && (!mustWait))
		sem_post(&block);
	else
		sem_post(&mutex);

	cout << threadName << "I leave so " << emptySeat << " seats are empty now" << endl;
}