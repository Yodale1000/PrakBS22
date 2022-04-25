//
// Created by Denitsa on 22.04.2022.
//
#include <stdio.h>
#include "keyValueStore.h"
#define MAX_LENGTH_KVS 100
#include <string.h>

/*Die put() Funktion soll eine Wert (value) mit dem Schlüsselwert (key) hinterlegen.
 * Wenn der Schlüssel bereits vorhanden ist,
 * soll der Wert überschrieben werden.
 * Der Rückgabewert der Funktion könnte Auskunft dazu geben.*/
int put(char *key, char *value, struct keyValueStore *kvs){
    int i;
    for( i = 0;i<MAX_LENGTH_KVS;i++){
        if(kvs[i].key == key || strlen(kvs[i].key) == 0){
            strcpy(kvs[i].value, value);
            strcpy(kvs[i].key,key);
            printf("PUT: %s%s",kvs[i].key,kvs[i].value);
            return 0;
        }
    }
    if(strcmp(key,"")|| strcmp(value,"")){
        printf("\nNo value ");
        return 1;
    }
    return 0;

}
//Die get() Funktion soll einen Schlüsselwert (key) in der Datenhaltung suchen
//und den hinterlegten Wert (value) zurückgeben. Ist der Wert nicht vorhanden,
//wird durch einen Rückgabewert <0 darauf aufmerksam gemacht.

int get(char *key, char *res, struct keyValueStore *kvs){
    int i;
    for( i = 0;i<MAX_LENGTH_KVS;i++){
        if(strcmp(kvs[i].key,key)){
           printf("Value: %s",kvs[i].value);
            return 0 ;
        }
        else{
            return -1;
        }
    }
    return 1;
}
//Die del() Funktion soll einen Schlüsselwert suchen
//und zusammen mit dem Wert aus der Datenhaltung entfernen.
int del(char *key, struct keyValueStore *kvs){
    int i;
    for( i = 0;i<MAX_LENGTH_KVS;i++) {
        if (strcmp(kvs[i].key, key)) {
            strcpy(kvs[i].key, "");
            strcpy(kvs[i].value, "");
        }
        else{
            printf("No key found");
        }
    }
    return 0;
}