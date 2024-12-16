#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include "utils.h"



struct Server {
    char ip[255];
    int port;
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

bool ConvertStringToUI64(const char *str, uint64_t *val) {
    char *end = NULL;
    unsigned long long i = strtoull(str, &end, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Out of uint64_t range: %s\n", str);
        return false;
    }
    if (errno != 0)
        return false;
    *val = i;
    return true;
}

void ReadServersFromFile(const char *filename, struct Server **servers, int *num_servers) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open servers file");
        exit(EXIT_FAILURE);
    }

    char line[255];
    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        *servers = realloc(*servers, sizeof(struct Server) * (count + 1));
        sscanf(line, "%s %d", (*servers)[count].ip, &(*servers)[count].port);
        count++;
    }

    *num_servers = count;
    fclose(file);
}

int main(int argc, char **argv) {
    uint64_t k = -1;
    uint64_t mod = -1;
    char servers_file[255] = {'\0'};

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"k", required_argument, 0, 0},
            {"mod", required_argument, 0, 0},
            {"servers", required_argument, 0, 0},
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
                ConvertStringToUI64(optarg, &k);
                break;
            case 1:
                ConvertStringToUI64(optarg, &mod);
                break;
            case 2:
                memcpy(servers_file, optarg, strlen(optarg));
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
        } break;

        case '?':
            printf("Arguments error\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || mod == -1 || !strlen(servers_file)) {
        fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n", argv[0]);
        return 1;
    }

    struct Server *servers = NULL;
    int num_servers = 0;
    ReadServersFromFile(servers_file, &servers, &num_servers);

    uint64_t total_result = 1;
    pthread_t *threads = malloc(sizeof(pthread_t) * num_servers);
    for (int i = 0; i < num_servers; i++) {
        // Создание сокета для каждого сервера
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(servers[i].port);
        struct hostent *hostname = gethostbyname(servers[i].ip);
        if (hostname == NULL) {
            fprintf(stderr, "gethostbyname failed with %s\n", servers[i].ip);
            exit(1);
        }
        server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);

        int sck = socket(AF_INET, SOCK_STREAM, 0);
        if (sck < 0) {
            fprintf(stderr, "Socket creation failed!\n");
            exit(1);
        }

        if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
            fprintf(stderr, "Connection failed\n");
            exit(1);
        }

        // Распределение задач между серверами
        uint64_t chunk_size = k / num_servers;
        uint64_t begin = i * chunk_size + 1;
        uint64_t end = (i == num_servers - 1) ? k : (begin + chunk_size - 1);

        char task[sizeof(uint64_t) * 3];
        memcpy(task, &begin, sizeof(uint64_t));
        memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
        memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

        if (send(sck, task, sizeof(task), 0) < 0) {
            fprintf(stderr, "Send failed\n");
            exit(1);
        }

        uint64_t response;
        if (recv(sck, &response, sizeof(response), 0) < 0) {
            fprintf(stderr, "Receive failed\n");
            exit(1);
        }

        total_result = MultModulo(total_result, response, mod);
        close(sck);
    }

    printf("Final result: %lu\n", total_result);
    free(servers);
    free(threads);
    return 0;
}