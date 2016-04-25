#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#include "main.h"

sem_t *shrek;
sem_t *fiona;
int basia;
struct memory *kasia = (struct memory *) -1;

void handler(int sig) {
    exit(0);
}

void clean() {
    if (shrek >= 0) {
        sem_close(shrek);
        sem_unlink("/shrek");
    }
    if (fiona >= 0) {
        sem_close(fiona);
        sem_unlink("/fiona");
    }
    if (basia >= 0) {
        close(basia);
        shm_unlink("/sysop");
    }
}

int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);
    srand(time(NULL));

    basia = shm_open("/sysop", O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    if (basia < 0 || ftruncate(basia, MEM_SIZE) < 0 ||
        (kasia = (struct memory *) mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, basia, 0)) < 0) {
        perror(NULL);
        exit(1);
    }
    for (int i = 0; i < SIZE; i++) {
        kasia->array[i] = rand() % MAX_NUM;
    }
    munmap(kasia, MEM_SIZE);
    shrek = sem_open("/shrek", O_CREAT, S_IWUSR | S_IRUSR, 1);
    fiona = sem_open("/fiona", O_CREAT, S_IWUSR | S_IRUSR, MAX_READERS);
    if (shrek < 0 || fiona < 0) {
        perror(NULL);
        exit(2);
    }

    while (1)
        pause();
}