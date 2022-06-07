#include <string.h>
#include <stdlib.h>
#include "subscriberStore.h"
#include <sys/socket.h>
#include <stdio.h>
#include "keyValueStore.h"
//erstes Element

subscriber *first = 0;

//Prüfe ob ein Subscriber in der Liste ist
int check_if_subscriber_on_list(char *key) {
    //erstes Element
    subscriber *pSubscriber = first;
    //ja, nicht in List
    int subscr_not_in_list=0;
    //gehe durch die Liste bis zum Ende
    while (pSubscriber != 0) {
        //wenn der übergebene key  nicht mit dem gespeicherten key übereinstimmt, return 1(nichts gefunden)
        if (strcmp(key, pSubscriber->key) == 0) {
            //susbcriber on the list
            subscr_not_in_list=-1;
        }
        //gehe zum nächsten Subscriber und prüfe das Ganze noch mal
        pSubscriber = pSubscriber->next;
    }
    //wenn key gefunden bei einem subscriber
    printf("check_if_subscriber_on_list %d.", subscr_not_in_list);
    return subscr_not_in_list;
}
//ein Subscriber zu der Subscriber Liste hinzufügen
void add_subscriber(char *key) {
    if(check_if_subscriber_on_list(key) == 0) {
        subscriber *next = first;
        first = malloc(sizeof(subscriber));
        size_t size = strlen(key) + 1;
        first->key = malloc(size);
        strcpy(first->key, key);
        first->pid = 0;
        first->next = next;
        printf("\nSubsriber hinzugefügt: %s", first->key);
    }
}

//lösche alles
void clear_subscribers() {
    //esrter Susbscriber nehmen
    subscriber *pSubscriber = first;
    //gehe durch die Liste bis zum Ende
    while (pSubscriber != 0) {
        //lösche den key des gefundenen Subscribers
        free(pSubscriber->key);
        pSubscriber->pid=0;
        subscriber *next = pSubscriber->next;
        //Lösche seinen Nachbar
        free(pSubscriber);
        pSubscriber = next;
    }
}


void notify(char *key, int connection, char *message){
    printf("Notify");
    char final_message [100];
    initialize_message_array(final_message, sizeof final_message);
//    subscriber *pSubscriber = first;

    //wenn subscr in der Liste --> benachrichtge ihn
    if(check_if_subscriber_on_list(key) == -1){
//        snprintf(final_message, sizeof(final_message), "\nFound\n");
//        send(connection, final_message, sizeof(final_message), 0);
//        printf("\nKey %s changed.", key);
        snprintf(final_message, sizeof(final_message), "\nSubscriber Message received: %s\n", message);
        send(connection, final_message, sizeof(final_message), 0);
    }
//    while (pSubscriber != 0) {
//        snprintf(final_message, sizeof(final_message), "\nWHile chleife in notify");
//        send(connection, final_message, sizeof(final_message), 0);
//        printf("\nSubscriber key %s",pSubscriber->key);
//        if (strcmp(key, pSubscriber->key)==0) {
//
//        }
//        pSubscriber = pSubscriber->next;
//    }
}