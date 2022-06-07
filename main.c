#include <stdio.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "sub.h"
#include <sys/sem.h>
#include "subscriptionStore.h"
#include "main.h"

#define PORT 5678


int main() {
    //Rendezvous Deskriptor, den wir anlegen und von dem eine
    //Verbindung entgegengenommen werden sollen
    int socket;

    // Connection Deksriptor, den wir öffnen wenn wir die Verbindung akzeptieren.
    // Randezvous lauscht weiter nach Verbindungen
    int connection;

    socket = create_socket(AF_INET, SOCK_STREAM, 0);
    bind_socket(&socket, PORT);
    listen_socket(&socket);
    printf("Socket ready\n");

    //Shared memory erzeugen
    //mmap dient zur Abbildung zwischen einem Prozessadressraum einer Datei
    struct keyValueStore *data_store = mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    struct subscriptionStore *sub_store = mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    static sem_t sem;
//    const char semname[] = "/tmp/mysem";
    sem_init(&sem, 1, 1);

    int semid;
    int marker[1];
    marker[0] = 1;
    semid = semget(IPC_PRIVATE,1,IPC_CREAT|0777);
    semctl(semid,1,SETALL,marker);


    //Server Schleife
    do {
        accept_socket(&socket, &connection);
        send(connection, "Connected\n", sizeof("Connected\n"), 0);

        for (int i = 0; i < MAX_LENGTH_SS; i++) {
            struct node *node = (struct node*)(deleteMessage());
            if (strcmp(sub_store[i].key, node) == 0) {
                send(connection, node->message, sizeof(message), 0);

            }

        }


        //erstelle Kinderprozesse
        int pid = fork();
        printf("%i", pid);
        if (pid == 0) {
            int i = 0;
            //mysemp = sem_open(semname, O_CREAT, 0644, 1);
            while (i != 2) {
                i=exec(prepare_input(&connection), &connection, data_store, sub_store, semid, getpid());
            }
            //sem_destroy(&sem);
            //Vaterprozess
        } else {
            close(connection);
        }
    } while (1);

}
