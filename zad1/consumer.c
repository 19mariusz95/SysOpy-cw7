#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <zconf.h>
#include "main.h"

int tom;
int jerry;
struct tu154m *jk;


void clean() {
    shmdt(jk);
}

void handler(int sig) {
    exit(0);
}

int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);

    tom = shmget(135, (SIZE + 1) * sizeof(int), S_IRUSR);
    jerry = semget(123, 2, S_IWUSR | S_IRUSR);
    if (tom <= 0 || jerry < 0) {
        perror(NULL);
        exit(1);
    }

    jk = shmat(tom, NULL, 0);
    if (jk == (void *) -1) {
        perror(NULL);
        exit(2);
    }
    struct sembuf krysia;
    krysia.sem_flg = 0;
    int t;
    int tn;
    int index;
    srand(time(NULL));
    struct timeval time;
    while (1) {
        krysia.sem_num = 0;
        krysia.sem_op = -1;
        semop(jerry, &krysia, 1);

        krysia.sem_num = 2;
        semop(jerry, &krysia, 1);
        index = jk->index;
        jk->index = (index + 1) % SIZE;
        krysia.sem_op = 1;
        semop(jerry, &krysia, 1);

        t = jk->array[index];
        krysia.sem_op = 1;
        krysia.sem_num = 1;
        semop(jerry, &krysia, 1);
        tn = semctl(jerry, 0, GETVAL, NULL);
        gettimeofday(&time, NULL);
        if (t % 2 == 0) {
            printf("(%d %ld,%ld) Sprawdzilem liczbe: %d na pozycji %d - parzysta. Pozostalo zadan oczekujacych %d\n",
                   getpid(), time.tv_sec, time.tv_usec / 1000, t, index, tn);
        }
        else {
            printf("(%d %ld,%ld) Sprawdzilem liczbe: %d na pozycji %d - nieparzysta. Pozostalo zadan oczekujacych %d\n",
                   getpid(), time.tv_sec, time.tv_usec / 1000, t, index, tn);
        }
        sleep(0.01);
    }
}