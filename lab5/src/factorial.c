#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int start;
    int end;
    int mod;
    long long *result;
    pthread_mutex_t *mutex;
} ThreadArgs;

// Функция для параллельного вычисления факториала на заданном диапазоне
void *partial_factorial(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    long long partial_result = 1;

    for (int i = threadArgs->start; i <= threadArgs->end; i++) {
        partial_result = (partial_result * i) % threadArgs->mod;
    }

    // Синхронизация доступа к общему результату
    pthread_mutex_lock(threadArgs->mutex);
    *(threadArgs->result) = (*(threadArgs->result) * partial_result) % threadArgs->mod;
    pthread_mutex_unlock(threadArgs->mutex);

    return NULL;
}

// Функция для разбора аргументов командной строки
int parse_args(int argc, char **argv, int *k, int *pnum, int *mod) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0) {
            if (i + 1 < argc) {
                *k = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: Missing value for -k\n");
                return 1;
            }
        } else if (strncmp(argv[i], "--pnum=", 7) == 0) {
            *pnum = atoi(argv[i] + 7);
        } else if (strncmp(argv[i], "--mod=", 6) == 0) {
            *mod = atoi(argv[i] + 6);
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return 1;
        }
    }

    // Проверка корректности параметров
    if (*k <= 0 || *pnum <= 0 || *mod <= 0) {
        fprintf(stderr, "Error: All arguments (-k, --pnum, --mod) must be positive integers.\n");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int k = -1, pnum = -1, mod = -1;

    if (parse_args(argc, argv, &k, &pnum, &mod) != 0) {
        return 1;
    }

    printf("Arguments parsed successfully: k = %d, pnum = %d, mod = %d\n", k, pnum, mod);

    pthread_t threads[pnum];
    ThreadArgs threadArgs[pnum];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    long long result = 1;

    int range = k / pnum;
    int remainder = k % pnum;

    // Создание потоков для параллельного вычисления
    for (int i = 0; i < pnum; i++) {
        threadArgs[i].start = i * range + 1;
        threadArgs[i].end = (i == pnum - 1) ? (i + 1) * range + remainder : (i + 1) * range;
        threadArgs[i].mod = mod;
        threadArgs[i].result = &result;
        threadArgs[i].mutex = &mutex;

        if (pthread_create(&threads[i], NULL, partial_factorial, &threadArgs[i]) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    // Ожидание завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            return 1;
        }
    }

    printf("Factorial mod %d is: %lld\n", mod, result);
    return 0;
}