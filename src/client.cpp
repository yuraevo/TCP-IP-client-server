#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define ERROR_C "ClIENT ERROR: "
#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT 1603
#define BUFFER_SIZE 1024
#define SERVER_CLOSE_CONNECTION_SYMBOL '#'

bool is_client_connection_close(const char* msg);

int main(int argc, char const* argvp[])
{
    int client = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[BUFFER_SIZE];
    bool isClose = false;

    if (client < 0)
    {
        std::cout << ERROR_C << "establishing socket error.";
        exit(0);
    }

    std::cout << "\n=> BusinessLogic socket created\n";

    struct sockaddr_in server_address;
    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    int ret = connect(client, 
        reinterpret_cast<const struct sockaddr*>(&server_address),
        sizeof(server_address));

    if(ret == 0)
    {
        std::cout << "=> Connection to server " 
        << inet_ntoa(server_address.sin_addr) 
        << "with port number: " << DEFAULT_PORT << "\n";
    }

    do
    {
        std::cout << "Client: ";
        bzero(buffer, BUFFER_SIZE);
        std::cin.getline(buffer, BUFFER_SIZE);
        send(client, buffer, strlen(buffer), 0);
        std::cout << buffer << std::endl;
        if(is_client_connection_close(buffer))
        {
            isClose = is_client_connection_close(buffer);
        }

        bzero(buffer, 1024);
        recv(client, buffer, sizeof(buffer), 0);
        printf("Server: %s\n", buffer);
        
    } while (isClose == false);
    
 
    // bzero(buffer, 1024);
    // strcpy(buffer, "HELLO, THIS IS CLIENT.");
    // printf("Client: %s\n", buffer);
    // send(business_logic, buffer, strlen(buffer), 0);
 
    // bzero(buffer, 1024);
    // recv(business_logic, buffer, sizeof(buffer), 0);
    // printf("Server: %s\n", buffer);
 
    // close(business_logic);
    // printf("Disconnected from the server.\n");
    return 0;

}
bool is_client_connection_close(const char* msg)
{
    for (int i = 0; i < strlen(msg); ++i)
    {
        if (msg[i] == SERVER_CLOSE_CONNECTION_SYMBOL)
        {
            return true;
        }
    }
    return false;
}