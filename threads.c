#include <stdio.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int flag = 0;

int getID() {
	static int counter = -1;
	pthread_mutex_lock(&lock);
	int result = ++counter;
	pthread_mutex_unlock(&lock);
	return result;
}

void* thread_func(void* param) {
	int id = getID();
	printf("[ThreadFunc]Thread #%lu with ID %d\n",pthread_self(),id);
	while(flag!=1);
	return 0;
}

void flag_switch(int sig) {
	flag = 1;
}

#define n 5

int main() {
	pthread_t threads[n];

	signal(SIGINT, flag_switch);

	for(int i = 0; i < n; i++) {
		pthread_create(&(threads[i]),NULL,thread_func,NULL);
	}
	for(int i = 0; i < n; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}
