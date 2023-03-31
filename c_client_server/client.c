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
    char buf[BUF_SIZE];
    struct sockaddr_in server;
    int server_socket;

    printf("Starting client...\n");

    printf("1. Creating socket...\n");
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    printf("   Socket created.\n");

    printf("2. Connecting to server...\n");
    connect(server_socket, (struct sockaddr *)&server, sizeof server);
    printf("   Server connected.\n");

    printf("3. Generating message to be sent...\n");
    sprintf(buf, "This is client %d.", getpid());
    printf("   Message: %s\n", buf);

    printf("4. Sending message to server...\n");
    write(server_socket, buf, BUF_SIZE);
    printf("   Message sent.\n");

    printf("5. Receiving message from server...\n");
    read(server_socket, buf, BUF_SIZE);
    printf("   Message from server: %s\n", buf);

    printf("6. Mission complete, closing socket...\n");
    close(server_socket);
    printf("   Socket closed, exit\n");
    return 0;
}