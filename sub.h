

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
//wir müssen selbst einen Typ für den Socket definieren
#define socket_t int

void error_exit(char *message);
/**
 * @param domain = Protokollfamilie, die benutzt werden soll
 * @param type = Typ der Kommunikation (verb.orientiert/-los)
 * @param protocol = genaues Protokoll (TCP/UDP)
 * **/
int create_socket( int domain, int type, int protocol );

/**
 * @param sock ist unser Socket
 * @param address = die Adresse, an der das Socket gebunden ist(aus IP und Port)
 * @param add_len = groesse des Structs
 * **/
void bind_socket(socket_t *sock, unsigned short port);

/**
 *
 * @param sock unser Socket(File Deskriptor)
 * @param wrtschl_length Länge der Warteschlange
 */
void listen_socket( socket_t *sock, int wrtschl_length );

/**
 *
 * @param sock = Filedeskriptor des Sockets, von dem eine Verbindung entegengenommen wird
 * @param new_socket = Fildeskriptor der angenommenen Verbindung
 */
void accept_socket( socket_t *sock, socket_t *new_socket );
/**
 *
 * @param soket = Socket, das geschlossen werden soll
 */
void close_socket(socket_t *socket);

/**
 *
 * @param sock Filedeskriptor, über den wir die Daten senden wollen
 * @param data ein Zeiger auf den Speicherbereich erwartet, in dem sich die Daten befinden
 * @param size Größe des Pakets das geschickt wird
 */
void send_data( socket_t *sock, char *data, size_t size);

/**
 *
 * @param sock Socket-Deskriptor der Verbindung
 * @param data Zeiger auf einen Puffer, in den die Daten gelegt werden sollen
 * @param size Länge des Puffers
 */
void recv_data( socket_t *sock, char *data, size_t size_puffer);


#endif //PRAKBS22_SUB_H
