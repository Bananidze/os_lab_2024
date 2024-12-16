#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include "utils.h"



struct FactorialArgs {
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
};

// uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
//     uint64_t result = 0;
//     a = a % mod;
//     while (b > 0) {
//         if (b % 2 == 1)
//             result = (result + a) % mod;
//         a = (a * 2) % mod;
//         b /= 2;
//     }
//     return result % mod;
// }

uint64_t Factorial(const struct FactorialArgs *args) {
    uint64_t ans = 1;
    for (uint64_t i = args->begin; i <= args->end; i++) {
        ans = MultModulo(ans, i, args->mod);
    }
    return ans;
}

void *ThreadFactorial(void *args) {
    struct FactorialArgs *fargs = (struct FactorialArgs *)args;
    uint64_t *result = malloc(sizeof(uint64_t));
    *result = Factorial(fargs);
    return (void *)result;
}

int main(int argc, char **argv) {
    int tnum = -1;
    int port = -1;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"port", required_argument, 0, 0},
            {"tnum", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 0: {
            switch (option_index) {
            case 0:
                port = atoi(optarg);
                break;
            case 1:
                tnum = atoi(optarg);
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
        } break;

        case '?':
            printf("Unknown argument\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (port == -1 || tnum == -1) {
        fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
        return 1;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Unable to create socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 3);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        uint64_t task[3];
        recv(new_socket, task, sizeof(task), 0);

        struct FactorialArgs fargs;
        fargs.begin = task[0];
        fargs.end = task[1];
        fargs.mod = task[2];

        pthread_t threads[tnum];
        uint64_t results[tnum];

        for (int i = 0; i < tnum; i++) {
            uint64_t range = (fargs.end - fargs.begin + 1) / tnum;
            uint64_t start = fargs.begin + i * range;
            uint64_t end = (i == tnum - 1) ? fargs.end : (start + range - 1);

            struct FactorialArgs *thread_args = malloc(sizeof(struct FactorialArgs));
            *thread_args = (struct FactorialArgs){.begin = start, .end = end, .mod = fargs.mod};

            pthread_create(&threads[i], NULL, ThreadFactorial, thread_args);
        }

        for (int i = 0; i < tnum; i++) {
            void *result;
            pthread_join(threads[i], &result);
            results[i] = *(uint64_t *)result;
            free(result);
        }

        uint64_t final_result = 1;
        for (int i = 0; i < tnum; i++) {
            final_result = MultModulo(final_result, results[i], fargs.mod);
        }

        send(new_socket, &final_result, sizeof(final_result), 0);
        close(new_socket);
    }

    return 0;
}