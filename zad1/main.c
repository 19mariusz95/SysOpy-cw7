#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <bits/signum.h>
#include <signal.h>
#include <zconf.h>
#include "main.h"

int basia;
int kasia;

void clean() {
    if (basia > 0)
        semctl(basia, 0, IPC_RMID);
    if (kasia > 0)
        semctl(kasia, 0, IPC_RMID);
}

void handler(int sig) {
    exit(0);
}

int main(int argc,char *argv[]){
    atexit(clean);
    signal(SIGINT, handler);
    basia = shmget(135, (SIZE + 1) * sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR);
    kasia = semget(123, 3, IPC_CREAT | S_IWUSR | S_IRUSR);
    if (basia < 0 || kasia < 0) {
        perror(NULL);
        exit(1);
    }
    union semun ss;
    ss.val = 0;
    semctl(kasia, 0, SETVAL, ss);
    ss.val = 1;
    semctl(kasia, 2, SETVAL, ss);
    ss.val = SIZE;
    semctl(kasia, 1, SETVAL, ss);

    while (1)
        pause();
    return 0;
}