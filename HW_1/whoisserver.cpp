//Tuan Vo
//CPSC 4510
//HW_1 -> server

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

#define MAXBUFF 1024

// alocate the port using student id
int allocate_port(const string &student_id = "4154412")
{
    size_t hash_value = stoul(student_id);
    int port_number = static_cast<int>(hash_value % 100);
    port_number += 10401;
    return port_number;
}

int main(int argc, char *argv[])
{
    int port;
    // if there any other arugment specified in command line
    // the port will assign based on the result of allocate_port();
    if (argc == 1)
        port = allocate_port();
    else if (argc == 2)
        port = atoi(argv[1]);
    else
    {
        cerr << "Usage: ./server -OR- ./server port" << endl;
        exit(1);
    }

    // create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0)
    {
        perror("socket failed");
        exit(1);
    }

    // define server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // check if bind works
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    // Allow up to 10 incoming requests and also check if listen works
    if (listen(server_socket, 20) < 0)
    {
        perror("Listen failed");
        exit(1);
    }
    cout << "Server listening on port: " << port << endl;

    while (true)
    {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket < 0)
        {
            cerr << "FAILED to accept client connection" << endl;
            continue;
        }

        // return ip-adrees of the client who connect to server
        char s[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), s, INET_ADDRSTRLEN);
        cout << "server got connection from " << s << endl;

        // Fork processing
        pid_t pid = fork();
        if (pid == -1)
        {
            cerr << "Failed to process fork()" << endl;
            continue;
        }
        else if (pid == 0)
        {
            char buffer[MAXBUFF*2];
            int recieve = recv(client_socket, buffer, MAXBUFF, 0);
            if (recieve == -1)
            {
                perror("recv failed");
                close(client_socket);
                continue;
            }
            
            //Report the command recived from client
            buffer[recieve] = '\0';
            cout << "Buffer received: " << buffer << endl;

            //check if the command is exactly whois
            string message = string(buffer, recieve);
            string first_word;
            size_t space = message.find(' ');
            if (space != string::npos)
                first_word = message.substr(0, space);

            if (first_word != "whois")
            {
                cerr << "Internal Error: the command is not supported. Please use whois command!" << endl;
                exit(1);
            }

            // Direct output to the socket
            if (dup2(client_socket, STDOUT_FILENO) == -1 || dup2(client_socket, STDERR_FILENO) == -1)
            {
                perror("dup2 failed");
                close(client_socket);
                continue;
            }

            // Execute command
            if (execl("/bin/sh", "sh", "-c", buffer, (char *)NULL) < 0)
            {
                perror("execl failed");
                close(client_socket);
                continue;
            }
        }
        else
            close(client_socket);
    }
    close(server_socket);
    return 0;
}