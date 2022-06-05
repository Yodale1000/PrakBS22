

#ifndef PRAKBS22_SUB_H
#define PRAKBS22_SUB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>
//#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "keyValueStore.h"

//wir müssen selbst einen Typ für den Socket definieren
#define socket_t int
#define MAX_COMMAND_LENGTH 10
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100

struct input {
    char command[MAX_COMMAND_LENGTH];
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
};

struct msgBuf
{
    long mtype;
    char mtext[100];
    char key[100];
    int commandtype;
};

struct input *prepare_input(const int *connection);

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
 * @param add_len = Größe des Structs
 * */
void bind_socket(socket_t *sock, unsigned short port);

/**
 *
 * @param sock unser Socket(File Deskriptor)
 */
void listen_socket(const socket_t *sock);

/**
 *
 * @param sock = File Deskriptor des Sockets, von dem eine Verbindung entgegengenommen wird
 * @param new_socket = File Deskriptor der angenommenen Verbindung
 */
void accept_socket(const socket_t *sock, socket_t *new_socket);

void up(int semid);
void down(int semid);

/**
 *
 * @param input unser Input
 * @param connection Verbindung´s Deskriptor, über den die Verbindung stattfindet
 * @param key_val Datenhaltung, wo den Zugriff auf Daten stattfindet
 * @return -1 im Fehlerfall
 */
int exec(struct input *input, const int *connection, struct keyValueStore *key_val, int semid, int msgid);

void message_loop(const int connection, int msgid);
void add_sub_message_loop(const int connection, int msgid);

#endif //PRAKBS22_SUB_H
