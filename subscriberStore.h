

#ifndef PRAKBS22_SUBSCRIBERSTORE_H
#define PRAKBS22_SUBSCRIBERSTORE_H
//#include "keyValueStore.h"
//Konstrukt für ein subscriber in der linked List
typedef struct subscriber {
    char *key;
    int pid;
    struct subscriber *next;
} subscriber;
/**
 *
 * @param key
 * @return 1 when key not found and 0 when key found
 */
int check_if_subscriber_on_list(char *key);
/**
 *
 * @param key
 * @param pid
 * füge Subscriber hinzu
 */
void add_subscriber(char *key);
/**
 * lösche die Liste
 */
void clear_subscribers();
void notify(char *key, int connection, char *message);
#endif //PRAKBS22_SUBSCRIBERSTORE_H
