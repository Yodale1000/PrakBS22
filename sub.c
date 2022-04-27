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
    const int y = 1;
    new_socket = socket(domain,type,protocol);

    //prüfen ob socket valid
    if(new_socket < 0){
        error_exit("Socket ist nicht valide");
    }
    //SO_REUSEADDR erlaubt dass mehrere Clients denselben Port teilen
    // Mehrere Clients können  mit dem Server in Verbindung treten.
    // VL : für schnelles Wiederverwenden gebundener Adressen
    //SOL_SOCKET ist zum Verwalten der Optionen auf Socket-Niveau
    setsockopt( new_socket, SOL_SOCKET,
                SO_REUSEADDR, &y, sizeof(int));
    return new_socket;

}

void bind_socket (socket_t *sock, unsigned long address, unsigned short add_len ){
    //an diese Adresse wird dann an das Socket gebunden
    //one of several types of socket address descriptors, with
    // different members depending on the network protocol
    struct sockaddr_in server;
    // kopiert eine bestimmte Anzahl(3.Arg) von bestimmten chars(2.Arg) von
    // einem String(1.Arg) zu dem selben String(1.Arg)
    memset( &server, 0, sizeof (server));
    //immer AF_INET
    server.sin_family = AF_INET;
    //INADDR_ANY binds the socket to all available interfaces.
    server.sin_addr.s_addr = INADDR_ANY;
    //konvertiert die vorzeichenlose kurze Ganzzahl hostshort von  Rechner-
    //nach Netzwerk-Byte-Ordnung.
    server.sin_port = htons(5678);

    //binden und prüfen, ob das Socket gebunden werden kann
    // bind würde ein Wert <0 liefern, wenn nicht erfolgreich
    if (bind(*sock, (struct sockaddr*)&server,sizeof(server)) < 0)
        error_exit("Das Socket wird nicht gebunden");
}

//Socket wird mitgeteilt, dass sich Clients verbinden wollen
void listen_socket( socket_t *sock ) {
    //prüfen ob listen fehlerhaft ist
    // -1 fehlerhaft, 0 erfolgreich
    if(listen(*sock, 5) == -1 ) { error_exit("Fehler"); }
}