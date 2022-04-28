
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "keyValueStore.h"
#define MAX_LENGTH_KVS 100


/*Die put() Funktion soll einen Wert (value) mit dem Schlüsselwert (key) hinterlegen.
 * Wenn der Schlüssel bereits vorhanden ist,
 * soll der Wert überschrieben werden.
 * Der Rückgabewert der Funktion könnte Auskunft dazu geben.*/
int put(char *key, char *value, struct keyValueStore *kvs){
    int i;

    //leeres Input prüfen
    if(strcmp(key,"")==0|| strcmp(value,"")==0){
        printf("\nNo key or value ");
        return 1;
    }

    for( i = 0; i<MAX_LENGTH_KVS; i++){
        //key vorhanden-> überschreiben oder key nicht vorhanden-> schreiben
        if(kvs[i].key == key || strlen(kvs[i].key) == 0){
            strcpy(kvs[i].value, value);
            strcpy(kvs[i].key,key);
            printf("PUT: %s:%s",kvs[i].key,kvs[i].value);
            return 0;
        }
    }

    return 0;

}
//Die get() Funktion soll einen Schlüsselwert (key) in der Datenhaltung suchen
//und den hinterlegten Wert (value) zurückgeben. Ist der Wert nicht vorhanden,
//wird durch einen Rückgabewert <0 darauf aufmerksam gemacht.

int get(char *key, struct keyValueStore *kvs){
    int i;
    //leeres Input prüfen
    if(strcmp(key,"")==0){
        printf("\nNo key found ");
        return 1;
    }
    for( i = 0; i<MAX_LENGTH_KVS; i++){
        if(strcmp(kvs[i].key,key)==0){
           printf("GET: %s:%s",kvs[i].key,kvs[i].value);
           //etwas gefunden
           return (int) kvs[i].value;
        }

    }
    //nichts gefunden
    printf("GET: %s:key_nonexistent",key);
    return -1;
}
//Die del() Funktion soll einen Schlüsselwert suchen
//und zusammen mit dem Wert aus der Datenhaltung entfernen.
int del(char *key, struct keyValueStore *kvs){
    int i;

    if(strcmp(key,"") == 0){
        printf("\nNo key found ");
        return -1;
    }
    for( i = 0;i<MAX_LENGTH_KVS;i++) {
        if (strcmp(kvs[i].key, key)) {
            strcpy(kvs[i].key, "");
            strcpy(kvs[i].value, "");
            return 0;
        }
    }
    printf("DEL: %s:key_nonexistent",key);
    return -1;
}