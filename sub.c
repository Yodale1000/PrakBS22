#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include "sub.h"

//Fehler behandeln und Fehlermeldung ausgeben
void error_exit(char *message){
    //stderr ist Standardfehlerausgabe
    printf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}
//Socket erstellen

int create_socket(int domain, int  type, int protocol){
    socket_t new_socket;
    new_socket = socket(domain,type,protocol);

    //prüfen ob socket valid
    if(new_socket < 0){
        error_exit("Socket ist nicht valide");
    }
    //SO_REUSEADDR erlaubt dass mehrere Clients denselben Port teilen
    // Mehrere Clients können  mit dem Server in Verbindung treten.
    //SOL_SOCKET ist zum Verwalten der Optionen auf Socket-Niveau
    setsockopt( new_socket, SOL_SOCKET,
                SO_REUSEADDR, &y, sizeof(int));
    return new_socket;

}


