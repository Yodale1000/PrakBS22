#include <stdio.h>
#include <sys/mman.h>
#include "sub.h"

#define PORT 5678

int main() {
    int socket;
    int connection;

    socket = create_socket(AF_INET, SOCK_STREAM, 0);
    bind_socket(&socket, PORT);
    listen_socket(&socket);
    printf("Socket ready\n");

    struct keyValueStore *shared_memory = mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                                               MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    do {
        accept_socket(&socket, &connection);
        printf("Client connected\n");
        send(connection, "Connected\n", sizeof("Connected\n"), 0);

        if (fork() == 0) {
            while (exec(prepare_input(&connection), &connection, shared_memory) != 2) {
            }
        } else {
            close(connection);
        }
    } while (1);
}
