#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1;
pthread_mutex_t lock2;

void *thread_func1(void *arg) {
    printf("Thread 1: Attempting to lock mutex 1...\n");
    pthread_mutex_lock(&lock1);
    printf("Thread 1: Locked mutex 1\n");

    sleep(1); // Имитируем работу, чтобы вызвать deadlock

    printf("Thread 1: Attempting to lock mutex 2...\n");
    pthread_mutex_lock(&lock2);
    printf("Thread 1: Locked mutex 2\n");

    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);

    printf("Thread 1: Finished\n");
    return NULL;
}

void *thread_func2(void *arg) {
    printf("Thread 2: Attempting to lock mutex 2...\n");
    pthread_mutex_lock(&lock2);
    printf("Thread 2: Locked mutex 2\n");

    sleep(1); // Имитируем работу, чтобы вызвать deadlock

    printf("Thread 2: Attempting to lock mutex 1...\n");
    pthread_mutex_lock(&lock1);
    printf("Thread 2: Locked mutex 1\n");

    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);

    printf("Thread 2: Finished\n");
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);

    pthread_create(&thread1, NULL, thread_func1, NULL);
    pthread_create(&thread2, NULL, thread_func2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);

    return 0;
}
