#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "sub.h"
#include "keyValueStore.h"

#define MAX_INPUT_LENGTH 100
#define MAX_COMMAND_LENGTH 10
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100

//Fehler behandeln und Fehlermeldung ausgeben
void error_exit(char *message) {
    //stderr ist Standardfehlerausgabe
    //Fehler mit dem Fehlercode von errno auswerten
    printf((const char *) stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

//Socket erstellen

int create_socket(int domain, int type, int protocol) {
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

void bind_socket(socket_t *sock, unsigned short port) {
    //an diese Adresse wird dann an das Socket gebunden
    //die Adresse besteht aus einigen Datenfeldern (siehe Readme)
    struct sockaddr_in address_of_server;
    //stellt sicher, es ist erstmal leer
    memset(&address_of_server, 0, sizeof(address_of_server));
    //immer AF_INET --> Protokollfamilie ipv4
    address_of_server.sin_family = AF_INET;
    //INADDR_ANY sagt dass jede IP-Adresse ist gültig
    //mit htonl zu Network Byte Order konvertieren
    address_of_server.sin_addr.s_addr = htonl(INADDR_ANY);
    //Port mittels htons ins Network Byte Order konveriteren
    address_of_server.sin_port = htons(port);

    //binden und prüfen, ob das Socket gebunden werden kann
    // bind würde ein Wert <0 liefern, wenn nicht erfolgreich
    if (bind(*sock, (struct sockaddr *) &address_of_server, sizeof(address_of_server)) < 0)
        error_exit("Das Socket wird nicht gebunden");
}

//Socket wird mitgeteilt, dass sich Clients verbinden wollen
void listen_socket(socket_t *sock) {
    //prüfen ob listen fehlerhaft ist
    // -1 fehlerhaft, 0 erfolgreich
    if (listen(*sock, 5) == -1) { error_exit("Fehler bei listen"); }
}

void accept_socket(socket_t *sock, socket_t *new_socket) {
    //Struktur mit der Infos zum Client
    struct sockaddr_in client;
    //die Länge der Struktur, die für Client übergeben wird
    unsigned int length_address_of_client;

    length_address_of_client = sizeof(client);
    //mit Pointer auf Struktur mit der Infos zum Client durch &client
    *new_socket = accept(*sock, (struct sockaddr *) &client, &length_address_of_client);
    //Filedeskriptor ist im Fehlerfall -1
    if (*new_socket == -1) { error_exit("Fehler bei accept. Verbindung wird nicht angenommen"); }
}

void close_socket(socket_t *socket) {
    if (close(*socket) < 0) {
        error_exit("Fehler beim Schließen der Verbindung");
    }
}

// Daten versenden via TCP
void send_data(socket_t *socket, char *data, size_t size) {
    //return wert von send ist Anzahl der Bytes, die gesendet (kann 0 sein) bzw. empfangen wurde
    //falls <0 ist Fehler aufgetreten
    if (send(*socket, data, size, 0) == -1)
        error_exit("Fehler bei send");
}

// Daten empfangen via TCP
void recv_data(const socket_t *socket, char *data, size_t size_puffer) {
    unsigned int length;
    //return Wert von recv ist ssize_t ( signed size_t = Größe vom Objekt im Speicher)
    length = recv(*socket, data, size_puffer, 0);
    if (length > 0)
        data[length] = '\0';
    else
        error_exit("Fehler bei recv()");
}

//Input
struct input *prepare_input(const int *connection) {
    char command[MAX_COMMAND_LENGTH] = {};
    char key[MAX_KEY_LENGTH] = {};
    char value[MAX_VALUE_LENGTH] = {};
    char messageInput[MAX_INPUT_LENGTH] = {};

    static struct input input;
    //Position des " " speichern
    int position[] = {0, 0, 0};
    //Position in den Strings command, key value speichern
    int counter_comm, counter_key, counter_val;
    int i;
    counter_comm = 0;
    counter_key = 0;
    counter_val = 0;

    //Daten empfangen
    recv(*connection, messageInput, sizeof(messageInput),0);
    messageInput[strlen(messageInput) - 2] = ' ';


    //solange wir nicht an das Ende des Strings sind, suche nach ' ' , wenn gefunden setze die Position
    for (i = 0; i < strlen(messageInput); i++) {
        if (messageInput[i] == ' ') {
            position[counter_comm++] = i;
        }
    }
    //suche die Position und speichere den Inhalt in command(jedes char anfangend bei i vom input, endend bei position[0]
    //gehe zur nächsten freien Stelle in command
    for (i= 0; i < position[0]; i++) {
        command[i] = messageInput[i];
    }
    //Speichere mir jedes Char von input anfangend bei i, endend bei position[1] ( oben gesetzt) ins key
    //gehe zur nächsten freien Stelle in key
    for (i = position[0] + 1; i < position[1]; i++) {
        key[counter_key++] = messageInput[i];
    }
    //gehe bis zum Ende des Inputs und speichere mir den Rest in value
    //wenn ich bereits in key habe
    if (position[1] != 0) {
        for (i = position[1] + 1; i < strlen(messageInput) - 2; i++) {
            value[counter_val++] = messageInput[i];
        }
    }

    strcpy(input.command, command);
    strcpy(input.key, key);
    strcpy(input.value, value);
    //printf("%s,%s,%s", command, key, value);
    return &input;
}


int exec(struct input *in, const int *connection, struct keyValueStore *key_val) {
    if (strcmp(in->command, "GET") == 0) {
        return get(in->key, key_val, *connection);
    } else if (strcmp(in->command, "PUT") == 0) {
        return put(in->key, in->value, key_val, *connection);
    } else if (strcmp(in->command, "DEL") == 0) {
        return del(in->key, key_val, *connection);
    } else if (strcmp(in->command, "QUIT") == 0) {
        close(*connection);
        printf("Disconnected\n");
        return 2;
    } else {
         printf("Invalid Input\n");
         if (send(*connection, "Invalid Input\n", sizeof("Invalid Input\n"), 0) == -1) {
             printf("Error occurred at send\n");
         }
         return 0;
    }
}