#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#define CICLOS 10

char * pais[3] = {
  "Peru",
  "Bolvia",
  "Colombia"
};

sem_t *SEM;

void proceso(int i) {
  int k;

  for (k = 0; k < CICLOS; k++) {
    // Entrada a la sección crítica
    sem_wait(SEM);
    printf("Entra %s", pais[i]);
    fflush(stdout);
    sleep(rand() % 3);
    printf("- %s Sale\n", pais[i]);
    // Salida de la sección crítica
    // Espera aleatoria fuera de la sección crítica
    sem_post(SEM);
    sleep(rand() % 3);
  }
  exit(0);
  // Termina el proceso
}
int main() {
  int pid;
  int status;
  int args[3];
  int i;
  
  SEM = sem_open("SEM", O_CREAT | O_EXCL, 0666, 1);

  srand(getpid());
  for (i = 0; i < 3; i++) {
    // Crea un nuevo proceso hijo que ejecuta la función proceso()
    pid = fork();
    if (pid == 0)
      proceso(i);
  }
  for (i = 0; i < 3; i++)
    pid = wait( & status);
  sem_destroy(SEM);
	
}