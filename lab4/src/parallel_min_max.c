#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>
#include "utils.h"

#define FILE_NAME_FORMAT "minmax_%d.txt"

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;
    int timeout = -1; // Таймаут по умолчанию

    while (true) {
        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 0}, // Добавляем опцию для таймаута
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        break;
                    case 3:
                        with_files = true;
                        break;
                    case 4:
                        timeout = atoi(optarg); // Получаем значение таймаута
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
         printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"] \n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    int pipe_fd[2];
    if (!with_files) {
        if (pipe(pipe_fd) == -1) {
            perror("pipe failed");
            return 1;
        }
    }

    pid_t child_pids[pnum]; // Массив для хранения PID дочерних процессов

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            // успешный fork
            active_child_processes += 1;
            child_pids[i] = child_pid; // Сохраняем PID дочернего процесса
            if (child_pid == 0) {
                // дочерний процесс
                int chunk_size = array_size / pnum;
                int start_index = i * chunk_size;
                int end_index = (i == pnum - 1) ? array_size : start_index + chunk_size;

                int min = INT_MAX;
                int max = INT_MIN;

                for (int j = start_index; j < end_index; j++) {
                    if (array[j] < min) min = array[j];
                    if (array[j] > max) max = array[j];
                }

                if (with_files) {
                    char filename[50];
                   snprintf(filename, sizeof(filename), FILE_NAME_FORMAT, i);
                    FILE *file = fopen(filename, "w");
                    fprintf(file, "%d %d\n", min, max);
                    fclose(file);
                } else {
                    write(pipe_fd[1], &min, sizeof(min));
                    write(pipe_fd[1], &max, sizeof(max));
                }
                exit(0);
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    // Закрываем запись в pipe после создания всех процессов
    if (!with_files) {
        close(pipe_fd[1]);
    }

    // Установка таймера на таймаут
    if (timeout > 0) {
        sleep(timeout); // Ждем указанный таймаут
        for (int i = 0; i < active_child_processes; i++) {
            kill(child_pids[i], SIGKILL);
        }
    }

    int global_min = INT_MAX;
    int global_max = INT_MIN;

    // Чтение результатов от дочерних процессов
    for (int i = 0; i < active_child_processes; i++) {
        if (!with_files) {
            int min, max;
            read(pipe_fd[0], &min, sizeof(min));
            read(pipe_fd[0], &max, sizeof(max));
            if (min < global_min) global_min = min;
            if (max > global_max) global_max = max;
        } else {
            char filename[50];
            snprintf(filename, sizeof(filename), FILE_NAME_FORMAT, i);
            FILE *file = fopen(filename, "r");
            if (file) {
                int min, max;
                fscanf(file, "%d %d", &min, &max);
                fclose(file);
                if (min < global_min) global_min = min;
                if (max > global_max) global_max = max;
            } else {
                perror("Failed to open file");
                return 1;
            }
        }
    }

    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);

    printf("Global min: %d\n", global_min);
    printf("Global max: %d\n", global_max);
    printf("Elapsed time: %ld microseconds\n", elapsed_time);

    free(array);
    return 0;
}