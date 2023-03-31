#define SERVER "tmp/server"
#define SERVER_PORT 4709
#define BUF_SIZE 2048

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int server_socket;
    struct sockaddr_in server;
    int client_socket;
    struct sockaddr_in client;
    char buffer[BUF_SIZE];

    printf("Starting server...\n");

    printf("1. Creating socket...\n");
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    printf("   Socket created.\n");

    printf("2. Binding address...\n");
    bind(server_socket, (struct sockaddr *)&server, sizeof server);
    printf("   Binded.\n");

    printf("3. Listening to port...\n");
    listen(server_socket, 20);
    printf("  Listening.\n");
    sleep(100);
    printf("sleep over.\n");

    socklen_t length = sizeof(client);
    printf("4. Waiting for client connection.\n");
//    client_socket = accept(server_socket, (struct sockaddr *)&client, &length);
    printf("   One client connected.\n");

    printf("4. Receiving message from client...\n");
    recv(client_socket, buffer, BUF_SIZE, 0);
    printf("   Message received: %s\n", buffer);

    printf("5. Responing client...\n");
    write(client_socket, buffer, BUF_SIZE);
    printf("   Message sent: %s\n", buffer);

    printf("6. Closing client socket...\n");
    close(client_socket);
    printf("   Socket closed.\n");

    printf("7. Closing server socket...\n");
    close(server_socket);
    printf("   Socket closed.\n");
    return 0;
}
