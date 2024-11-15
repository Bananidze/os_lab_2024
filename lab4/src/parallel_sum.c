#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include "utils.h" // Предполагается, что здесь находится объявление GenerateArray
#include "sum.h"   // Заголовочный файл для функции Sum

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char *argv[]) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    // Обработка аргументов командной строки
    int opt;
    while ((opt = getopt(argc, argv, "t:s:a:")) != -1) {
        switch (opt) {
            case 't':
                threads_num = atoi(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
            case 'a':
                array_size = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -t threads_num -s seed -a array_sizen", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (threads_num == 0 || array_size == 0) {
        fprintf(stderr, "Threads number and array size must be greater than 0.n");
        exit(EXIT_FAILURE);
    }

    int *array = malloc(sizeof(int) * array_size);
    if (array == NULL) {
        perror("Failed to allocate memory for the array");
        return 1;
    }

    GenerateArray(array, array_size, seed);

    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];

    clock_t start_time = clock();
    // Разделение работы между потоками
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * (array_size / threads_num);
        args[i].end = (i + 1) * (array_size / threads_num);
        if (i == threads_num - 1) {
            args[i].end = array_size; // Последний поток обрабатывает оставшиеся элементы
        }
        
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            printf("Error: pthread_create failed!n");
            free(array);
            return 1;
        }
    }

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_taken);
    free(array);
    printf("Total: %d\n", total_sum);
    return 0;
}