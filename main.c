#include <stdio.h>
#include <sys/mman.h>
#include "sub.h"
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define PORT 5678

int main() {
    //Rendezvous Deskriptor, den wir anlegen und von dem eine
    //Verbindung entgegengenommen werden sollen
    int socket;

    // Connection Deskriptor, den wir öffnen, wenn wir die Verbindung akzeptieren.
    // Rendezvous lauscht weiter nach Verbindungen
    int connection;

    socket = create_socket(AF_INET, SOCK_STREAM, 0);
    bind_socket(&socket, PORT);
    listen_socket(&socket);
    printf("Socket ready\n");

    //Shared memory erzeugen
    //mmap dient zur Abbildung zwischen einem Prozessadressraum einer Datei
    struct keyValueStore *data_store = mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //die Zuordnung = Subscription von einem msgid (etwas wie ClientID) zu einem key
    struct subscription *subscription_store = mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int msgid;

    static sem_t sem;
//   initialisieren des Semaphores
    sem_init(&sem, 1, 1);

    //?? marker--> googeln
    int semid;
    int marker[1];
    marker[0] = 1;
    semid = semget(IPC_PRIVATE,1,IPC_CREAT|0777);
    semctl(semid,1,SETALL,marker);


    //Server Schleife
    do {
        accept_socket(&socket, &connection);
        send(connection, "Connected\n", sizeof("Connected\n"), 0);

        //erstelle Kinderprozesse
        if (fork() == 0) {
            //wir kennzeichnen ein Client damit, um mit ihm zu kommunizieren mit dem message queue
            msgid = msgget(IPC_PRIVATE,IPC_CREAT|0777);
            //Fehler auffangen
            if (msgid < 0) {
                perror("msgget");
                exit(1);
            }
            printf("%d", msgid);
            int i = 0;
            // Prozess erstellen, der prüft ob es neue Nachrichten in der message Queue gibt. Da  senden wir
            // bei put und delete zu message Queue dass diese Commands stattgefunden haben
            if (fork() == 0){
                while(1) {
                    message_loop(connection, msgid);
                }
            }

            while (i != 2) {
                i=exec(prepare_input(&connection), &connection, data_store, semid, msgid, subscription_store);
            }
            //sem_destroy(&sem);
            //Vaterprozess
        } else {
            close(connection);
        }
    } while (1);
}
