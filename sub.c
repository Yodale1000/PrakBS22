#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "sub.h"
#include "keyValueStore.h"
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define MAX_COMMAND_LENGTH 10
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100
#define MAX_INPUT_LENGTH 100

//so eine Art counter, damit der Semaphore weiß, ob es blockieren muss oder nicht
int has_exclusive_access = 0;
struct messageQueueElement *firstElement = 0;
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
    //SO_REUSEADDR erlaubt, dass mehrere Clients denselben Port teilen
    // mehrere Clients können mit dem Server in Verbindung treten.
    // VL: für schnelles Wiederverwenden gebundener Adressen
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
    //INADDR_ANY sagt, dass jede IP-Adresse ist gültig
    //mit htonl zu Network Byte Order konvertieren
    address_of_server.sin_addr.s_addr = htonl(INADDR_ANY);
    //Port mittels htons ins Network Byte Order konvertieren
    address_of_server.sin_port = htons(port);

    //binden und prüfen, ob das Socket gebunden werden kann
    // bind würde ein Wert <0 liefern, wenn nicht erfolgreich
    if (bind(*sock, (struct sockaddr *) &address_of_server, sizeof(address_of_server)) < 0)
        error_exit("Das Socket wird nicht gebunden");
}

//Socket wird mitgeteilt, dass sich Clients verbinden wollen
void listen_socket(const socket_t *sock) {
    //prüfen, ob listen fehlerhaft ist
    // -1 fehlerhaft, 0 erfolgreich
    if (listen(*sock, 5) == -1) { error_exit("Fehler bei listen"); }
}

void accept_socket(const socket_t *sock, socket_t *new_socket) {
    //Struktur mit den Informationen zum Client
    struct sockaddr_in client;
    //die Länge der Struktur, die für Client übergeben wird
    unsigned int length_address_of_client;


    length_address_of_client = sizeof(client);
    //mit Pointer auf Struktur mit den Informationen zum Client durch &client
    *new_socket = accept(*sock, (struct sockaddr *) &client, &length_address_of_client);
    //File deskriptor ist im Fehlerfall -1
    if (*new_socket == -1) { error_exit("Fehler bei accept. Verbindung wird nicht angenommen"); }
    printf("Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    printf("Client %d connected\n", ntohs(client.sin_port));

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
    recv(*connection, messageInput, sizeof(messageInput), 0);
    messageInput[strlen(messageInput) - 2] = ' ';


    //solange wir nicht an das Ende des Strings sind, suche nach ' ' , wenn gefunden setze die Position
    for (i = 0; i < strlen(messageInput); i++) {
        if (messageInput[i] == ' ') {
            position[counter_comm++] = i;
        }
    }
    //suche die Position und speichere den Inhalt in command(jedes char anfangend bei i vom input, endend bei position[0]
    //gehe zur nächsten freien Stelle in command
    for (i = 0; i < position[0]; i++) {
        command[i] = messageInput[i];
    }
    //Speichere mir jedes Char von input anfangend bei i, endend bei position[1] (oben gesetzt) ins key
    //gehe zur nächsten freien Stelle in key
    for (i = position[0] + 1; i < position[1]; i++) {
        key[counter_key++] = messageInput[i];
    }
    //gehe bis zum Ende des Inputs und speichere mir den Rest in value,
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
//semaphore frei geben
void up(int semid)
{
    struct sembuf semBuftmp;
    semBuftmp.sem_num = 0;
    semBuftmp.sem_op = 1;
    semBuftmp.sem_flg = 0;
    semop(semid,&semBuftmp,1);
}
//semaphore blockieren
void down(int semid)
{
    struct sembuf semBuftmp;
    semBuftmp.sem_num = 0;
    semBuftmp.sem_op = -1;
    semBuftmp.sem_flg = 0;
    semop(semid,&semBuftmp,1);
}


int exec(struct input *input, const int *connection, struct keyValueStore *key_val, int semid, int msgid, struct messageIds *msgIds) {
    if (strcmp(input->command, "BEG") == 0){
        printf("Befehl bekommen (BEG)\n");
        if (!has_exclusive_access) {
            printf("BEG:start_transaction\n");
            down(semid);
            has_exclusive_access = 1;
        }
    }
    else if (strcmp(input->command, "END") == 0){
        printf("Befehl bekommen (END)\n");
        if (has_exclusive_access) {
            printf("END:end_transaction\n");
            has_exclusive_access = 0;
            up(semid);
        }
    }
    else if (strcmp(input->command, "GET") == 0) {
        if(!has_exclusive_access) down(semid);
        int result = get(input->key, key_val, *connection);
        if(!has_exclusive_access) up(semid);
        return result;
    } else if (strcmp(input->command, "PUT") == 0) {
        if(!has_exclusive_access) down(semid);
        int result = put(input->key, input->value, key_val, *connection, msgid, msgIds);
        if(!has_exclusive_access) up(semid);
        return result;
    } else if (strcmp(input->command, "DEL") == 0) {
        if(!has_exclusive_access) down(semid);
        int result = del(input->key, key_val, *connection, msgid,msgIds);
        if(!has_exclusive_access) up(semid);
        return result;
    } else if (strcmp(input->command, "SUB") == 0) {
        if(!has_exclusive_access) down(semid);
        int result = sub(input->key, key_val, *connection, msgid,msgIds);

        if(!has_exclusive_access) up(semid);
        return result;
    } else if (strcmp(input->command, "QUIT") == 0) {
        printf("Disconnected\n");
        close(*connection);
        return 2;
    } else {
        printf("%s\n", input->command);
        printf("Invalid Input\n");
        if (send(*connection, "Invalid Input\n", sizeof("Invalid Input\n"), 0) == -1) {
            printf("Error occurred at send\n");
        }
        return 0;
    }

}

void message_loop(const int connection, int msgid){
    char message[100];
    initialize_message_array(message,sizeof message);
    struct msgBuf buf;
    msgrcv(msgid,&buf,sizeof(buf),2,0);
    if(buf.commandtype == 1){
        snprintf(message, sizeof(message), "\nAdd subscriber: %s\n", buf.key);
        send(connection, message, sizeof(message), 0);
        add_subscriber(buf.key);
    } else if (buf.commandtype == 0){
        notify(buf.key, connection, buf.mtext);
    }
    snprintf(message, sizeof(message), "\nMessage received: %s\n", buf.mtext);
    send(connection, message, sizeof(message), 0);
}


void add_sub_message_loop(const int connection, int msgid) {
//    send(connection, "add_sub_message_loop", sizeof("add_sub_message_loop"), 0);
//    char message[100];
//    struct msgBuf buf;
//    msgrcv(msgid, &buf, sizeof(buf), 3, 0);
//    add_subscriber(buf.key);
//    snprintf(message, sizeof(message), "\nSub Add Message received: %s\n", buf.mtext);
//    send(connection, message, sizeof(message), 0);
}


//Message Queue Methoden
void add_to_queue(int msgid, struct messageIds *msgIds, char *key){
    int ptr = msgIds->ptr;
    msgIds->msgids[ptr] = msgid;
    strcpy(msgIds[ptr].key,key);
    msgIds->ptr = msgIds->ptr + 1;
    printf("\nMsgId hinzugefügt: %d, %s", msgIds->msgids[ptr], msgIds[ptr].key);

//    int msg_already_in_queue=0;
//
//    for(int i=0;i<ptr;i++){
//        if(msgid==msgIds->msgids[ptr]){
//            msg_already_in_queue = -1;
//        }
//    }
//    if(msg_already_in_queue == 0) {
//        msgIds->msgids[ptr] = msgid;
//        strcpy(&msgIds->key[ptr],key);
//        msgIds->ptr = msgIds->ptr + 1;
//    }
//    struct messageQueueElement *next = firstElement;
//    firstElement = malloc(sizeof(messageQueueElement));
//    firstElement->msgid = msgid;
//    firstElement->next = next;
}

