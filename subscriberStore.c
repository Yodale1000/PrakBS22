#include <string.h>
#include <stdlib.h>
#include "subscriberStore.h"
#include <sys/socket.h>
#include <stdio.h>
#include "keyValueStore.h"
//erstes Element
subscriber *first = 0;

//Prüfe ob ein Subscriber in der Liste ist
int check_if_subscriber_on_list(char *key, subscriber first_subscriber ) {
    //erstes Element
    printf("Before");
    subscriber *pSubscriber = &first_subscriber;
    printf("After");
    //gehe durch die Liste bis zum Ende
    while (pSubscriber != 0) {
        //wenn der übergebene key  nicht mit dem gespeicherten key übereinstimmt, return 1(nichts gefunden)
        if (!strcmp(key, pSubscriber->key)) {
            return 1;
        }
        //gehe zum nächsten Subscriber und prüfe das Ganze noch mal
        pSubscriber = pSubscriber->next;
    }
    //wenn key gefunden bei einem subscriber
    return 0;
}
//ein Subscriber zu der Subscriber Liste hinzufügen
void add_subscriber(char *key, int pid, subscriber first_subscriber  ) {
    //
    subscriber *next= &first_subscriber;
    first=malloc(sizeof(subscriber));
    size_t size = strlen(key) + 1;
    first->key = malloc(size);
    strcpy(first->key, key);
    first->pid = pid;
    first->next = next;
    printf("\nSubsriber hinzugefügt: %s %d", first->key, first->pid);
}
//lösche alles
void clear_subscribers() {
    //esrter Susbscriber nehmen
    subscriber *pSubscriber = first;
    //gehe durch die Liste bis zum Ende
    while (pSubscriber != 0) {
        //lösche den key des gefundenen Subscribers
        free(pSubscriber->key);
        //??
        pSubscriber->pid=0;
        subscriber *next = pSubscriber->next;
        //Lösche seinen Nachbar
        free(pSubscriber);
        pSubscriber = next;
    }
}
void notify(char *key,int connection, subscriber first_subscriber ){
    printf("\nIch bin in Notify.");
    char message[200];
    //send(connection,"\nIch bin in Notify",sizeof message,0);

    subscriber *pSubscriber = &first_subscriber;

    while (pSubscriber != 0) {
        printf("\nSubscriber key %s %d",pSubscriber->key, pSubscriber->pid);
        //snprintf(message, sizeof(message), "GET: %d \n", 2);
        //send(connection,message,sizeof message,0);
        if (strcmp(key, pSubscriber->key)==0) {
            printf("\nKey %s changed.", key);
            //printf("\nIch bin in notify und habe den key");
            //send(connection,"Ich bin in notify und habe den key",sizeof message,0);
            int conid= pSubscriber->pid;
            snprintf(message, sizeof(message), "\nKey %s changed.", key);
            send(conid, message, sizeof(message), 0);

        }
        pSubscriber = pSubscriber->next;
    }
}