
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "keyValueStore.h"
#include "sub.h"

#define MAX_LENGTH_KVS 100


/*Die put() Funktion soll einen Wert (value) mit dem Schlüsselwert (key) hinterlegen.
 * Wenn der Schlüssel bereits vorhanden ist,
 * soll der Wert überschrieben werden.
 * Der Rückgabewert der Funktion könnte Auskunft dazu geben.*/
int put(char *key, char *value, struct keyValueStore *kvs, int connection){
    int i;
    char message[50];
    //leeres Input prüfen
    if(strcmp(key,"")==0|| strcmp(value,"")==0){
        printf("\nNo key or value ");
        snprintf(message,sizeof(message),"\nNo key found \n");
        send(connection, message, sizeof (message),0);
        return 1;
    }

    for( i = 0; i<MAX_LENGTH_KVS; i++){
        //key vorhanden-> überschreiben oder key nicht vorhanden-> schreiben
        if(kvs[i].key == key || strlen(kvs[i].key) == 0){
            strcpy(kvs[i].value, value);
            strcpy(kvs[i].key,key);
            //sprintf() wird dazu verwendet, um einen String aus einem Formatstring zu erzeugen
            snprintf(message,sizeof(message),"PUT: %s:%s\n",kvs[i].key,kvs[i].value);
            //wir müssen es mitteilen, was passiert ist
            send(connection, message, sizeof (message),0);
            return 0;
        }
    }
    return 0;
}

//Die get() Funktion soll einen Schlüsselwert (key) in der Datenhaltung suchen
//und den hinterlegten Wert (value) zurückgeben. Ist der Wert nicht vorhanden,
//wird durch einen Rückgabewert <0 darauf aufmerksam gemacht.

int get(char *key, struct keyValueStore *kvs,int connection){
    int i;
    char message[50];
    //leeres Input prüfen
    if(strcmp(key,"")==0){
        //printf("\nNo key found ");
        snprintf(message,sizeof(message),"\nNo key found \n");
        send(connection, message, sizeof (message), 0);
        return 1;
    }
    for( i = 0; i<MAX_LENGTH_KVS; i++){
        if(strcmp(kvs[i].key,key)==0){
           //printf("GET: %s:%s",kvs[i].key,kvs[i].value);

            snprintf(message,sizeof(message),"GET: %s:%s\n",kvs[i].key,kvs[i].value);
            send(connection, message, sizeof (message), 0);
           //etwas gefunden
           return (int) kvs[i].value;
        }

    }
    //nichts gefunden
    snprintf(message,sizeof(message),"GET: %s key_notfound\n",kvs[i].key);
    send(connection, message, sizeof (message), 0);
    return -1;
}

//Die del() Funktion soll einen Schlüsselwert suchen
//und zusammen mit dem Wert aus der Datenhaltung entfernen.
int del(char *key, struct keyValueStore *kvs, int connection){
    int i;
    char deleted_key[100];
    char message[50];
    if(strcmp(key,"") == 0){
        printf("\nNo key found\n ");
        return -1;
    }
    for( i = 0;i<MAX_LENGTH_KVS;i++) {
        if (strcmp(kvs[i].key, key) == 0) {
            strcpy(deleted_key,kvs[i].key);
            strcpy(kvs[i].key, "");
            strcpy(kvs[i].value, "");
            snprintf(message, sizeof message, "DEL:%s:key_deleted\n", deleted_key);
            send(connection, message, sizeof (message), 0);
            return 0;
        }
    }
    snprintf(message, sizeof(message), "DEL:%s:key_nonexistent\n", key);
    send(connection, message, sizeof (message), 0);
    return -1;
}