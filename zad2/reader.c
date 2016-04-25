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
#include <string.h>
#include "main.h"

sem_t *fiona;
int basia;
struct memory *kasia = (struct memory *) -1;
int sum = 0, total = 0, n;

void clean() {
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

void handler(int sig) {
    if (sum) {
        printf("%d Znalazlem %d liczb: %d.\n", getpid(), total, n);
    }
    exit(0);
}

int read_args(int argc, char *argv[], int *x, int *only_summary) {
    if (argc != 2 && argc != 3) {
        printf("1 or 2 arguments expected");
        exit(1);
    }
    *only_summary = 0;
    if (argc == 3) {
        if (strcmp(argv[2], "-u") == 0) {
            *only_summary = 1;
        }
        else {
            exit(2);
        }
    }
    *x = atoi(argv[1]);
    return 0;
}

int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);
    if (read_args(argc, argv, &n, &sum) != 0) {
        exit(3);
    }

    srand(time(NULL));

    basia = shm_open("/sysop", O_RDWR, 0);
    if (basia < 0 || (kasia = (struct memory *) mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, basia, 0)) < 0) {
        perror(NULL);
        exit(4);
    }

    fiona = sem_open("/fiona", 0);
    if (fiona < 0) {
        perror(NULL);
        exit(4);
    }

    int counter;
    struct timeval tval;
    while (1) {
        counter = 0;
        if (sem_wait(fiona) < 0) {
            perror(NULL);
            exit(5);
        }
        for (int i = 0; i < SIZE; i++) {
            if (kasia->array[i] == n) {
                counter++;
            }
        }
        total += counter;
        gettimeofday(&tval, NULL);
        if (sum == 0)
            printf("%d %ld.%ld Znalazlem %d liczb: %d.\n", getpid(), tval.tv_sec, tval.tv_usec / 1000, counter, n);
        if (sem_post(fiona) < 0) {
            perror(NULL);
            exit(6);
        }
        sleep(1);
    }
}