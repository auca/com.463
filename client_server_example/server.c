#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <time.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_NAME "time server"

#define SERVER_PORT 7777
#define MAX_CONNECTIONS 1

#define REPLY_BUFFER_SIZE 1000
#define MESSAGE_MAX_SIZE  1000

void process_error(char *message);
int  close_socket(int socket);

int main(int argc, char **argv)
{
    int server_socket, client_socket;

    struct in_addr      server_addr_rep;
    struct sockaddr_in  server_address, client_address;

    socklen_t client_address_length;

    char message[MESSAGE_MAX_SIZE], reply[REPLY_BUFFER_SIZE];
    char *server_time;

    time_t server_time_in_ms;

    server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server_socket == -1)
    { process_error(NULL); exit(EXIT_FAILURE); }

    memset(&server_addr_rep, 0, sizeof(server_addr_rep));

    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (-1 == bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)))
    { process_error(NULL); close_socket(server_socket); exit(EXIT_FAILURE); }

    if (-1 == listen(server_socket, MAX_CONNECTIONS))
    { process_error(NULL); close_socket(server_socket); exit(EXIT_FAILURE); }

    printf("Server started on %s\n", inet_ntoa(server_address.sin_addr));

    for (;;)
    {
        client_address_length = sizeof(client_address);

        memset(&client_address, 0, client_address_length);

        if (-1 == (client_socket = accept(server_socket, (struct sockaddr *) &client_address, 
                                                                             &client_address_length)))
        { process_error(NULL); continue; }

        server_time_in_ms = time(NULL);
        server_time = ctime(&server_time_in_ms);

        memset(&message, 0, sizeof(message));
        sprintf(message, "%s\n%s", SERVER_NAME, server_time);

        if (-1 == send(client_socket, &message, strlen(message), 0))
        { process_error(NULL); close_socket(client_socket); continue; }

        memset(&reply, 0, sizeof(reply));

        if (-1 == recv(client_socket, &reply, sizeof(reply), MSG_WAITALL))
        { process_error(NULL); close_socket(client_socket); continue; }

        printf("Connection from: %s\n", inet_ntoa(client_address.sin_addr));
        printf("Message: %s\n\n", reply);

        if (-1 == close_socket(client_socket))
        { process_error(NULL); }
    }

    if (-1 == close_socket(server_socket))
    { exit(EXIT_FAILURE); }

    return EXIT_SUCCESS;
}

int close_socket(int socket)
{
    int result = -1;

    if (-1 == shutdown(socket, SHUT_RDWR))
    {
        process_error(NULL);
    }
    else
    {
        if (-1 == close(socket))
        {
            process_error(NULL);
        }
        else
        {
            result = 1;
        }
    }

    return result;
}

void process_error(char *message)
{
    if (message == NULL)
    {
        perror("Error");
    }
    else
    {
        fputs(message, stderr);
    }
}
