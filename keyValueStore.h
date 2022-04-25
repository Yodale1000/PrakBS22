//
// Created by Denitsa on 22.04.2022.
//

#ifndef PRAKBS22_KEYVALUESTORE_H
#define PRAKBS22_KEYVALUESTORE_H

//konstante max Größe für die Strings
#define MAX_LENGTH_VALUE 100
#define MAX_LENGTH_KEY 10


struct keyValueStore{
    char key[MAX_LENGTH_KEY];
    char value[MAX_LENGTH_VALUE];
};

int put(char* key, char* value, struct keyValueStore *kvs);
int get(char* key, char* res, struct keyValueStore *kvs);
int del(char* key, struct keyValueStore *kvs);

#endif //PRAKBS22_KEYVALUESTORE_H
