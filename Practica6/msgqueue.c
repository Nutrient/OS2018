#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/types.h>

#define CICLOS 10

char * pais[3] = {
  "Peru",
  "Bolvia",
  "Colombia"
};

typedef struct {
	long msg_type;		// Tipo de mensaje, debe ser long
	char mensaje[100];	// Contenido
	} MSGTYPE; 

int msgqid;
MSGTYPE emisor;
MSGTYPE receptor;

void proceso(int i) {
  int k;
  

  for (k = 0; k < CICLOS; k++) {
    // Entrada a la sección crítica
    
    msgrcv(msgqid, &receptor, sizeof(MSGTYPE)-sizeof(long), 1, 0);
    
    printf("Entra %s", pais[i]);
    fflush(stdout);
    sleep(rand() % 3);
    printf("- %s Sale\n", pais[i]);
    // Salida de la sección crítica
    // Espera aleatoria fuera de la sección crítica

    strcpy(emisor.mensaje,"Fin");
    msgsnd(msgqid,&emisor,sizeof(MSGTYPE)-sizeof(long),0);
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
  
  msgqid = msgget(0x1234, IPC_CREAT | 0666);
  emisor.msg_type = 1;
  strcpy(emisor.mensaje,"Start");
  msgsnd(msgqid,&emisor,sizeof(MSGTYPE)-sizeof(long),0);

  srand(getpid());
  for (i = 0; i < 3; i++) {
    // Crea un nuevo proceso hijo que ejecuta la función proceso()
    pid = fork();
    if (pid == 0)
      proceso(i);
  }
  for (i = 0; i < 3; i++)
    pid = wait( & status);
 
  msgctl(msgqid, IPC_RMID, NULL);
	
}