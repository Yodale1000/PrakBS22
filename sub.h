

#ifndef PRAKBS22_SUB_H
#define PRAKBS22_SUB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include "keyValueStore.h"

//wir müssen selbst einen Typ für den Socket definieren
#define socket_t int
#define MAX_COMMAND_LENGTH 10
#define MAX_KEY_LENGTH 10
#define MAX_VALUE_LENGTH 100

struct input {
    char command_s[MAX_COMMAND_LENGTH];
    char key_s[MAX_KEY_LENGTH];
    char value_s[MAX_VALUE_LENGTH];
};

struct input *input_func(const int *cfd);

void error_exit(char *message);

/**
 * @param domain = Protokollfamilie, die benutzt werden soll
 * @param type = Typ der Kommunikation (verb.orientiert/-los)
 * @param protocol = genaues Protokoll (TCP/UDP)
 * **/
int create_socket(int domain, int type, int protocol);

/**
 * @param sock ist unser Socket
 * @param address = die Adresse, an der das Socket gebunden ist(aus IP und Port)
 * @param add_len = groesse des Structs
 * **/
void bind_socket(socket_t *sock, unsigned short port);

/**
 *
 * @param sock unser Socket(File Deskriptor)
 */
void listen_socket(socket_t *sock);

/**
 *
 * @param sock = Filedeskriptor des Sockets, von dem eine Verbindung entegengenommen wird
 * @param new_socket = Fildeskriptor der angenommenen Verbindung
 */
void accept_socket(socket_t *sock, socket_t *new_socket);

/**
 *
 * @param socket = Socket, das geschlossen werden soll
 */
void close_socket(socket_t *socket);

/**
 *
 * @param sock Filedeskriptor, über den wir die Daten senden wollen
 * @param data ein Zeiger auf den Speicherbereich erwartet, in dem sich die Daten befinden
 * @param size Größe des Pakets das geschickt wird
 */
void send_data(socket_t *sock, char *data, size_t size);

/**
 *
 * @param sock Socket-Deskriptor der Verbindung
 * @param data Zeiger auf einen Puffer, in den die Daten gelegt werden sollen
 * @param size Länge des Puffers
 */
void recv_data(socket_t *sock, char *data, size_t size_puffer);


int exec(struct input *in, const int *connection, struct keyValueStore *key_val);

#endif //PRAKBS22_SUB_H
