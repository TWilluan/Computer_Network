// Tuan Vo
// CPSC 4510
// HW_1 -> client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>

#define MAXBUFF 1024

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        cerr << "Usage: whoisclient hostname:server whois [option] arugment-list" << endl;
        exit(1);
    }

    //create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == 0)
    {
        perror("socket client failed");
        exit(1);
    }

    char buffer[MAXBUFF];
    struct sockaddr_in server_address;

    // parse hostname:server and store in vector
    stringstream ss(argv[1]);
    vector<string> hostname_server;
    string attr;
    while (getline(ss, attr, ':'))
        hostname_server.push_back(attr);

    // Define server address and connect to server
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(stoi(hostname_server[1]));
    inet_pton(AF_INET, hostname_server[1].c_str(), &server_address.sin_addr);

    // Parse command line to get whois command
    stringstream temp;
    temp << argv[2];
    for (int i = 3; i < argc; i++)
        temp << " " << argv[i];
    string whois = temp.str();

    // check if client is able to connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        cerr << "Client Connect failed" << endl;
        exit(1);
    }

    // Send package to Server
    snprintf(buffer, MAXBUFF, "%s", whois.c_str());
    send(client_socket, buffer, strlen(buffer), 0);

    // Read message from Server
    while (true)
    {
        int recieve = recv(client_socket, buffer, MAXBUFF, 0);
        if (recieve <= 0)
        {
            cout << "Server disconnected - Thank you for using out network" << endl;
            break;
        }
        buffer[recieve] = '\0';
        cout << buffer << endl;
    }

    close(client_socket);
    return 0;
}