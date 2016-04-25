#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/time.h>
#include "main.h"

sem_t *shrek;
sem_t *fiona;
int basia;
struct memory *kasia = (struct memory *) -1;

void clean() {
    if (shrek >= 0) {
        sem_close(shrek);
    }
    if (fiona >= 0) {
        sem_close(fiona);
    }
    if (basia >= 0) {
        if (kasia >= 0) {
            munmap(kasia, MEM_SIZE);
        }
        close(basia);
    }
}

void handler(int signum) {
    exit(0);
}

int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);
    srand(time(NULL));

    basia = shm_open("/sysop", O_RDWR, 0);
    if (basia < 0) {
        perror(NULL);
        exit(1);
    }
    kasia = (struct memory *) mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, basia, 0);
    if (kasia < 0) {
        perror(NULL);
        exit(2);
    }

    shrek = sem_open("/shrek", 0);
    fiona = sem_open("/fiona", 0);
    if (shrek < 0 || fiona < 0) {
        perror(NULL);
        exit(2);
    }

    int si, number, index;
    struct timeval tval;
    while (1) {
        number = rand() % 100;
        si = rand() % MAX_NUM;
        for (int j = 0; j < number; j++) {
            if (sem_wait(shrek) < 0) {
                perror(NULL);
                exit(3);
            }

            for (int i = 0; i < MAX_READERS; i++) {
                if (sem_wait(fiona) < 0) {
                    perror(NULL);
                    exit(4);
                }
            }
            index = rand() % SIZE;
            kasia->array[index] = si;
            gettimeofday(&tval, NULL);
            printf("%d %ld.%ld Wpisalem liczbe %d na pozycje %d. Pozostalo %d zadan.\n", getpid(), tval.tv_sec,
                   tval.tv_usec / 1000, si, index, number - j - 1);
            si++;
            for (int i = 0; i < MAX_READERS; i++) {
                if (sem_post(fiona) < 0) {
                    perror(NULL);
                    exit(5);
                }
            }

            if (sem_post(shrek) < 0) {
                perror(NULL);
                exit(6);
            }
        }
        sleep(1);
    }
}