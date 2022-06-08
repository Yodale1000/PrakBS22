

#ifndef PRAKBS22_SUBSCRIBERSTORE_H
#define PRAKBS22_SUBSCRIBERSTORE_H

struct subscription {
    int subscriptions[50];
    char key[50];
    //ptr zeigt auf die Stelle des Arrays, die frei ist
    int ptr;
};

int check_if_in_list(struct subscription *subscriptions, char *key, int msgid);

void add_to_queue(int msgid, struct subscription *subscriptions, char *key);

void notify(char *message, char *key, struct subscription *subscriptions);

void add_message_to_queue(char *message, char *key, int msgid, int msgtype);

#endif //PRAKBS22_SUBSCRIBERSTORE_H
