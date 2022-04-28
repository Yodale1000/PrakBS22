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

int create_socket(int domain, int  type, int protocol) {
    socket_t new_socket;
    const int y = 1;
    new_socket = socket(domain, type, protocol);

    //prüfen ob socket valid
    if (new_socket < 0) {
        error_exit("Socket ist nicht valide");
    }
    //SO_REUSEADDR erlaubt dass mehrere Clients denselben Port teilen
    // Mehrere Clients können  mit dem Server in Verbindung treten.
    // VL : für schnelles Wiederverwenden gebundener Adressen
    //SOL_SOCKET ist zum Verwalten der Optionen auf Socket-Niveau
    setsockopt(new_socket, SOL_SOCKET,
               SO_REUSEADDR, &y, sizeof(int));
    return new_socket;
}
void bind_socket (socket_t *sock, unsigned short port ){
    //an diese Adresse wird dann an das Socket gebunden
    //die Adresse besteht aus einigen Datenfeldern (siehe Readme)
    struct sockaddr_in address_of_server;
    //stellt sicher, es ist erstmal leer
    memset(&address_of_server, 0, sizeof (address_of_server));
    //immer AF_INET --> Protokollfamilie ipv4
    address_of_server.sin_family = AF_INET;
    //INADDR_ANY sagt dass jede IP-Adresse ist gültig
    //mit htonl zu Network Byte Order konvertieren
    address_of_server.sin_addr.s_addr = htonl(INADDR_ANY);
    //Port mittels htons ins Network Byte Order konveriteren
    address_of_server.sin_port = htons(port);

    //binden und prüfen, ob das Socket gebunden werden kann
    // bind würde ein Wert <0 liefern, wenn nicht erfolgreich
    if (bind(*sock, (struct sockaddr*)&address_of_server, sizeof(address_of_server)) < 0)
        error_exit("Das Socket wird nicht gebunden");
}

//Socket wird mitgeteilt, dass sich Clients verbinden wollen
void listen_socket( socket_t *sock ) {
    //prüfen ob listen fehlerhaft ist
    // -1 fehlerhaft, 0 erfolgreich
    if(listen(*sock, 5) == -1 ) { error_exit("Fehler bei listen"); }
}
void accept_socket (socket_t *sock, socket_t *new_socket){
    //Struktur mit der Infos zum Client
    struct sockaddr_in client;
    //die Länge der Struktur, die für Client übergeben wird
    unsigned int len;

    len = sizeof(client);
    //mit Pointer auf Struktur mit der Infos zum Client durch &client
    *new_socket=accept(*sock,(struct sockaddr *)&client, &len);
    //Filedeskriptor ist im Fehlerfall -1
    if (*new_socket  == -1) { error_exit("Fehler bei accept"); }
}
void close_socket(socket_t *socket) {
    if (close(*socket) < 0) {
        error_exit("Fehler beim Schließen der Verbindung");
    }
}

