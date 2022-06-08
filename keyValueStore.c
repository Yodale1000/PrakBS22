
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
int put(char *key, char *value, struct keyValueStore *kvs, int connection, int msgid, struct subscription *subscriptions) {
    int i;
    char message[50];
    initialize_message_array(message, sizeof(message));
    //leeres Input prüfen
    if (strcmp(key, "") == 0 || strcmp(value, "") == 0) {
        printf("\nNo key or value ");
        snprintf(message, sizeof(message), "No key found \n");
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
            // wir gehen die subscription Liste durch und prfen ob es zu dem übergebenen key
            // eine Subscription existiert, wenn ja, wird der Subscriber benachrichtigt über
            //die Änderung
            notify(message, key, subscriptions);
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
        snprintf(message, sizeof(message), "No key found\n");
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
int del(char *key, struct keyValueStore *kvs, int connection, int msgid,struct subscription *subscriptions) {
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
            notify(message, key, subscriptions);
            return 0;
        }
    }
    snprintf(message, sizeof(message), "DEL:%s:key_nonexistent\n", key);
    // message[sizeof message] = '\0';
    send(connection, message, sizeof(message), 0);
    return -1;
}

int sub(char *key, struct keyValueStore *kvs, int connection, int msgid, struct subscription *subscriptions){
    int i;
    int check_key_found=0;
    char message[50];
    initialize_message_array(message, sizeof message);
    //leeres Input prüfen
    if (strcmp(key, "") == 0) {
        printf("\nNo key");
        snprintf(message, sizeof(message), "No key found \n");
        send(connection, message, sizeof(message), 0);
        return 1;
    }

    for (i = 0; i < MAX_LENGTH_KVS; i++) {
        if (strcmp(kvs[i].key, key) == 0) {
            check_key_found=1;
        }
    }
    if(check_key_found == 0){
        snprintf(message, sizeof(message), "Key not found, cannot subscribe\n");
        send(connection, message, sizeof(message), 0);
        return 0;
    }
    //wenn nicht in der subscription Liste
    // addiere den subscriber zu der subscriber Liste == Zuordnung hinzufügen
    if (check_if_in_list(subscriptions, key, msgid) == 0) {
        snprintf(message, sizeof(message), "SUB: %s\n", key);
        //wir mÜssen es mitteilen, was passiert ist
        send(connection, message, sizeof(message), 0);
        // add to Liste mit msgId und key, für die Client abonniert ist
        add_to_queue(msgid, subscriptions,key);
        return 0;
    }
    //Subscriber ist schon in der Liste
    else{
        snprintf(message, sizeof(message), "Bereits subscribed\n");
        send(connection, message, sizeof(message), 0);
        return 0;
    }

}
