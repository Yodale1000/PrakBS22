#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include "sub.h"

//prüft ob ein key und ein msgid in der Loste der subscriptions sind. Wenn ja -> -1 , wenn nein --> 0
int check_if_in_list(struct subscription *subscriptions, char *key, int msgid){
    int ptr = subscriptions->ptr;
    for(int i=0;i<ptr;i++){
        if( msgid == subscriptions->subscriptions[i] && strcmp(subscriptions[i].key,key) == 0){
            //in list
            return -1;
        }
    }
    //not in list
    return 0;
}

//Message Queue Methoden
void add_to_queue(int msgid, struct subscription *subscriptions, char *key){
    int ptr = subscriptions->ptr;
    subscriptions->subscriptions[ptr] = msgid;
    strcpy(subscriptions[ptr].key,key);
    subscriptions->ptr = subscriptions->ptr + 1;
}

//die Liste mit Susbscriber nach einer Zuprdnung suchen und
// zu der Message Queue hinzufügen. msgtype = 5 haben wir für SUB Command festgelegt
//und so wissen wir dass es sich um eine SUB Command handelt
void notify(char *message, char *key, struct subscription *subscriptions){
    for(int i=0;i<subscriptions->ptr;i++){
        if(strcmp(subscriptions[i].key,key) == 0) {
            add_message_to_queue(message, key, subscriptions->subscriptions[i], 5);
        }
    }
}
//hier speichern wir was der SUbscriber als Notification kriegen soll.
// smgsnd sendet die Notification an dem Subscriber
void add_message_to_queue(char *message, char *key, int msgid, int msgtype){
    struct msgBuf buf;
    buf.mtype = msgtype;
    strcpy(buf.mtext, message);
    strcpy(buf.key, key);
    msgsnd(msgid,&buf,sizeof(buf),0);
}
