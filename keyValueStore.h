//
// Created by Denitsa on 22.04.2022.
//

#ifndef PRAKBS22_KEYVALUESTORE_H
#define PRAKBS22_KEYVALUESTORE_H

//konstante max Größe für die Strings
#define MAX_LENGTH_VALUE 10
#define MAX_LENGTH_KEY 10
#include "subscriberStore.h"


struct keyValueStore{
    char key[MAX_LENGTH_KEY];
    char value[MAX_LENGTH_VALUE];
};
void initialize_message_array(char *message, size_t buff_len);
int put(char* key, char* value, struct keyValueStore *kvs, int connection);
int get(char* key, struct keyValueStore *kvs, int connection);
int del(char* key, struct keyValueStore *kvs, int connection);
int sub(char *key, struct keyValueStore *kvs, int connection,int pid,subscriber first_subscriber);
#endif //PRAKBS22_KEYVALUESTORE_H
