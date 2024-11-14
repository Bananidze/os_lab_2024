#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        printf("Дочерний процесс с PID %d завершён\n", getpid());
        exit(0);
    } else {
        printf("Родительский процесс с PID %d создал дочерний процесс с PID %d\n", getpid(), pid);
        printf("Теперь родительный процесс будет ожидать 10 секунд...\n");
        sleep(10);
        printf("Родительский процесс завершает выполнение\n");
    }
    return 0;
}