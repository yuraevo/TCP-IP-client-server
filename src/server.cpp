#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>

#define ERROR_S "SERVER ERROR: "
#define DEFAULT_PORT 1603
#define DEFAULT_PORT_BL 1605
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'

std::mutex display_mutex;

bool is_client_connection_close(const char* msg);
void communication_with_client();

int main(int argc, char const* argvp[])
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << "ID of process running main program: " << this_id << std::endl;
    std::thread t1(communication_with_client);
    // std::thread t2(communication_with_business_logic);
    t1.join();
    // t2.join();
    std::cout << "Done" << std::endl;
}

void communication_with_client()
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << "ID of process running communication_with_client program: " << this_id << std::endl;
    display_mutex.lock();

    int client, server, business_logic;
    char buffer[BUFFER_SIZE];
    bool isExit = false;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0)
    {
        std::cout << ERROR_S << "Can't accepting client.\n";
    }
    printf("[+]TCP server socket created.\n");

    business_logic = socket(AF_INET, SOCK_STREAM, 0);
    if (business_logic < 0)
    {
        std::cout << ERROR_S << "Can't accepting business_logic.\n";
    }
    printf("[+]TCP business_logic socket created.\n");


    struct sockaddr_in server_address, client_addr, business_logic_addr;
    socklen_t size = sizeof(server_address);
    socklen_t size_bl = sizeof(business_logic_addr);
    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    memset(&business_logic_addr, '\0', sizeof(business_logic_addr));
    business_logic_addr.sin_port = htons(DEFAULT_PORT_BL);
    business_logic_addr.sin_family = AF_INET;
    business_logic_addr.sin_addr.s_addr = htons(INADDR_ANY);

    int ret = bind(server, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address));
    if (ret < 0)
    {
        std::cout << ERROR_S << "binding connection. Socket has already been establishing.\n";
        exit(1);
    }

    ret = bind(business_logic, reinterpret_cast<struct sockaddr*>(&business_logic_addr), sizeof(business_logic_addr));
    if (ret < 0)
    {
        std::cout << ERROR_S << "binding connection. Socket has already been establishing.\n";
        exit(1);
    }

    listen(server, 5);
    listen(business_logic, 5);
    printf("Listening...\n");

    while(true)
    {
        size = sizeof(size);
        size_bl = sizeof(size_bl);

        std::cout << "SERVER: " << "Listening clients...\n";
        client = accept(server, reinterpret_cast<struct sockaddr*>(&client_addr), &size);
        if(client < 0)
        {
            printf("[+]Client not connected.\n");

        }
        else {
             printf("[+]Client connected.\n");

            bzero(buffer, 1024);
            recv(client, buffer, sizeof(buffer), 0);
            printf("Client: %s\n", buffer);
        }

        business_logic = accept(business_logic, reinterpret_cast<struct sockaddr*>(&business_logic_addr), &size_bl);

        if(business_logic < 0) {
            
                printf("[+]business_logic not connected.\n");

            }
            else {
                
                printf("[+]business_logic connected.\n");
                send(business_logic, buffer, strlen(buffer), 0);
            }

        // server = accept(business_logic, reinterpret_cast<struct sockaddr*>(&server_address), &size);
 
        // bzero(buffer, 1024);
        // strcpy(buffer, "HI, THIS IS SERVER. Please wait, results are being processed");
        // printf("Server: %s\n", buffer);
        // send(client, buffer, strlen(buffer), 0);
 
        close(client);
        printf("[+]Client disconnected.\n\n");
        
    }
    display_mutex.unlock();
}

void communication_with_business_logic()
{

}

bool is_client_connection_close(const char* msg)
{
    for (int i = 0; i < strlen(msg); ++i)
    {
        if (msg[i] == CLIENT_CLOSE_CONNECTION_SYMBOL)
        {
            return true;
        }
    }
    return false;
}