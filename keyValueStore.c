
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <malloc.h>
#include "keyValueStore.h"
#include "subscriberStore.h"
#include "sub.h"
#include <sys/msg.h>
#define MAX_LENGTH_KVS 100
void initialize_message_array(char *message, size_t buff_len){
    int i;
    for(i=0; i< buff_len;i++){
        message[i] = '\0';
    }
}

/*Die put() Funktion soll einen Wert (value) mit dem Schlüsselwert (key) hinterlegen.
 * Wenn der Schlüssel bereits vorhanden ist,
 * soll der Wert überschrieben werden.
 * Der Rückgabewert der Funktion könnte Auskunft dazu geben.*/
int put(char *key, char *value, struct keyValueStore *kvs, int connection, int msgid, struct messageIds *msgIds) {
    int i;
    char message[50];
    initialize_message_array(message, sizeof(message));
    //leeres Input prüfen
    if (strcmp(key, "") == 0 || strcmp(value, "") == 0) {
        printf("\nNo key or value ");
        snprintf(message, sizeof(message), "\nNo key found \n");
        send(connection, message, sizeof(message), 0);
        return 1;
    }

    for (i = 0; i < MAX_LENGTH_KVS; i++) {
        //key vorhanden-> überschreiben oder key nicht vorhanden-> schreiben
        if (strcmp(kvs[i].key, key) == 0 || strlen(kvs[i].key) == 0) {
            strcpy(kvs[i].value, value);
            strcpy(kvs[i].key, key);
            //sprintf() wird dazu verwendet, um einen String aus einem Formatstring zu erzeugen
            snprintf(message, sizeof("PUT: %s:%s"), "PUT: %s:%s\n", kvs[i].key, kvs[i].value);
            //wir müssen es mitteilen, was passiert ist
            send(connection, message, sizeof(message), 0);
            // loop über messageIds in Message List & add_message auf allen msg_ids
            for(i=0;i<msgIds->ptr;i++){
                add_message_to_queue(message, key, msgIds->msgids[i], 2, 0);
            }

//            messageQueueElement *p=firstElement;
//            while(p !=0){
//                printf("MsgId in PUT:%d",msgid);
//                add_message_to_queue(message, key, p->msgid, 2, 0);
//                p=p->next;
//            }

            return 0;
        }
    }
    return 0;
}

//Die get() Funktion soll einen Schlüsselwert (key) in der Datenhaltung suchen
//und den hinterlegten Wert (value) zurückgeben. Ist der Wert nicht vorhanden,
//wird durch einen Rückgabewert <0 darauf aufmerksam gemacht.

int get(char *key, struct keyValueStore *kvs, int connection) {
    int i;
    char message[50];
    initialize_message_array(message, sizeof(message));
    //leeres Input prüfen
    if (strcmp(key, "") == 0) {
        snprintf(message, sizeof(message), "\nNo key found");
        send(connection, message, sizeof(message), 0);
        return 1;
    }
    for (i = 0; i < MAX_LENGTH_KVS; i++) {
        if (strcmp(kvs[i].key, key) == 0) {
            //printf("GET: %s:%s",kvs[i].key,kvs[i].value);
            snprintf(message, sizeof(message), "GET: %s:%s\n", kvs[i].key, kvs[i].value);
            send(connection, message, sizeof(message), 0);
            //etwas gefunden
            return (int) kvs[i].value;
        }

    }
    //nichts gefunden
    snprintf(message, sizeof(message), "GET: %s key_notfound\n", kvs[i].key);
    send(connection, message, sizeof(message), 0);
    return -1;
}

//Die del() Funktion soll einen Schlüsselwert suchen
//und zusammen mit dem Wert aus der Datenhaltung entfernen.
int del(char *key, struct keyValueStore *kvs, int connection, int msgid,struct messageIds *msgIds) {
    int i;
    char deleted_key[50];
    initialize_message_array(deleted_key, sizeof(deleted_key));
    char message[50];
    initialize_message_array(message, sizeof(message));
    if (strcmp(key, "") == 0) {
        printf("\nNo key found\n ");
        return -1;
    }
    for (i = 0; i < MAX_LENGTH_KVS; i++) {
        if (strcmp(kvs[i].key, key) == 0) {
            strcpy(deleted_key, kvs[i].key);
            strcpy(kvs[i].key, "");
            strcpy(kvs[i].value, "");
            snprintf(message, sizeof message, "DEL:%s:key_deleted\n", deleted_key);
            send(connection, message, sizeof(message), 0);
            for(i=0;i<msgIds->ptr;i++){
                add_message_to_queue(message, key, msgIds->msgids[i], 2, 0);
            }
            //add_message_to_queue(message, deleted_key, msgid, 2, 0);
            return 0;
        }
    }
    snprintf(message, sizeof(message), "DEL:%s:key_nonexistent\n", key);
    // message[sizeof message] = '\0';
    send(connection, message, sizeof(message), 0);
    return -1;
}

void add_message_to_queue(char *message, char *key, int msgid, int msgtype, int commandtype){
    struct msgBuf buf;
    buf.mtype = msgtype;
    buf.commandtype = commandtype;
    strcpy(buf.mtext, message);
    strcpy(buf.key, key);
    msgsnd(msgid,&buf,sizeof(buf),0);
}

int sub(char *key, struct keyValueStore *kvs, int connection, int msgid){
    int i;
    int check_key_found=0;
    char message[50];
    //leeres Input prüfen
    if (strcmp(key, "") == 0) {
        printf("\nNo key");
        snprintf(message, sizeof(message), "\nNo key found \n");
        send(connection, message, sizeof(message), 0);
        return 1;
    }
    for (i = 0; i < MAX_LENGTH_KVS; i++) {
        if (strcmp(kvs[i].key, key) == 0) {
            check_key_found=1;
        }
    }
    if(check_key_found == 0){
        snprintf(message, sizeof(message), "Key not found, cannot subscribe");
        send(connection, message, sizeof(message), 0);
        return 0;
    }
    if (check_if_subscriber_on_list(key) == 0) {
        snprintf(message, sizeof(message), "SUB: %s\n", key);
        //wir mÜssen es mitteilen, was passiert ist
        send(connection, message, sizeof(message), 0);
        add_message_to_queue(message, key, msgid, 2, 1);
        return 0;
    }
    else{
        snprintf(message, sizeof(message), "Bereits subscribed");
        send(connection, message, sizeof(message), 0);
        return 0;
    }

}

void put_message_from_queue_for_subscriber(messageQueueElement *firstElement){
//    messageQueueElement *p=firstElement;
//    while(p !=0){
//        add_to_queue(p->msgid);
//        p=p->next;
//    }
}