//
// Created by Michel on 6/4/2022.
//
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <malloc.h>
#include "subscriptionStore.h"


#define MAX_LENGTH_SS 100
#define MAX_LENGTH_KEY 10
#define MAX_LENGTH_MSG 50

struct node {
    char message[MAX_LENGTH_MSG];
    char key[MAX_LENGTH_KEY];
    struct node *next;
};

struct node *head = NULL;



void addMessage(char *key, char *message) {
    struct node *link = (struct node*) malloc(sizeof(struct node));
    strcpy(link->key, key);
    strcpy(link->message, message);
    link->next = head;
    head = link;
}

struct node* deleteMessage() {
    struct node *tempLink = head;
    head = head->next;
    return tempLink;
}

void printList() {
    struct node *ptr = head;
    printf("\n[ ");

    //start from the beginning
    while(ptr != NULL) {
        printf("(%s,%s) ",ptr->key,ptr->message);
        ptr = ptr->next;
    }

    printf(" ]");
}


int subscribe(char *key, int pid, struct subscriptionStore *ss, int connection) {
    int i;
    char message[50];
    for (i = 0; i < MAX_LENGTH_SS; i++) {
        if (strlen(ss[i].key) == 0 && ss[i].pid == 0) {
            ss[i].pid = pid;
            strcpy(ss[i].key, key);
            snprintf(message, sizeof(message), "SUB: %s\n", ss[i].key);
            send(connection, message, sizeof(message), 0);
            return 0;

        } else if (ss[i].pid == pid && strcmp(ss[i].key, key) == 0) {
            snprintf(message, sizeof(message), "SUB: already subscribed\n");
            send(connection, message, sizeof(message), 0);
            return 0;
        }
    }
    return 1;
}