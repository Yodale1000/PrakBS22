//
// Created by Michel on 6/4/2022.
//

#ifndef PRAKBS22_SUBSCRIPTIONSTORE_H
#define PRAKBS22_SUBSCRIPTIONSTORE_H
#define MAX_LENGTH_KEY 10


struct subscriptionStore{
    char key[MAX_LENGTH_KEY];
    int pid;
};

int subscribe(char* key, int pid, struct subscriptionStore *ss, int connection);

void addMessage(char *key, char *message);

struct node* deleteMessage();

void printList();

#endif //PRAKBS22_SUBSCRIPTIONSTORE_H
