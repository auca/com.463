#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>

#define SERVER_PORT 7777

#define REPLY_BUFFER_SIZE 1000

void process_error(char *message);
int  close_socket(int socket);

int main(int argc, char **argv)
{
    int client_socket;

    struct sockaddr_in server_address;

    struct hostent *host_ip_addresses;
    struct in_addr *host_ip_address;

    char reply[REPLY_BUFFER_SIZE];

    if (argc > 2)
    {
        if (!(host_ip_addresses = gethostbyname(argv[1])))
        { herror(NULL); exit(EXIT_FAILURE); }

        host_ip_address = (struct in_addr *) *(host_ip_addresses->h_addr_list);
        printf("Server IP: %s\n", inet_ntoa(*host_ip_address));

        client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (client_socket == -1)
        { process_error(NULL); exit(EXIT_FAILURE); }

        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port   = htons(SERVER_PORT);
        server_address.sin_addr   = *host_ip_address;

        if (-1 == connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)))
        { process_error(NULL); close_socket(client_socket); exit(EXIT_FAILURE); }

        if (-1 == send(client_socket, argv[2], strlen(argv[2]), 0))
        { process_error(NULL); close_socket(client_socket); exit(EXIT_FAILURE); }

        memset(&reply, 0, sizeof(reply));

        if (-1 == recv(client_socket, &reply, sizeof(reply), MSG_WAITALL))
        { process_error(NULL); close_socket(client_socket); exit(EXIT_FAILURE); }

        printf("Reply: %s\n", reply);

        if (-1 == close_socket(client_socket))
        { exit(EXIT_FAILURE); }
    }
    else
    {
        if (argc == 2)
        {
            process_error("The client message was not specified.");
        }
        else
        {
            process_error("Server IP address was not specified.");
        }
    }

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
