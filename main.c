#include <stdio.h>
#include <sys/mman.h>
#include "sub.h"

#define PORT 5678

int main() {
    //Randezvous Deskriptor, den wir anlegen und von dem eine
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
    //mmap  dient zur Abbildung zwischen einem Prozessadressraum einer Datei
    struct keyValueStore *data_store = mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    //Server Schleife
    do {
        accept_socket(&socket, &connection);
        send(connection, "Connected\n", sizeof("Connected\n"), 0);

        //erstelle Kinderprozesse
        if (fork() == 0) {
            while (exec(prepare_input(&connection), &connection, data_store) != 2) {
            }
        //Vaterprozess
        } else {
            close(connection);
        }
    } while (1);
}
