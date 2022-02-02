#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include <sstream>
#include <stdio.h>
#include <cstdlib> //std::system
#include <mutex>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <map>
#include <string>

#define ERROR_S "SERVER ERROR: "
#define DEFAULT_PORT 1603
#define DEFAULT_PORT_BL 1605
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'
#define BOOST_INTERPROCESS_BOOTSTAMP_IS_SESSION_MANAGER_BASED

typedef struct
{
  char symbol[BUFFER_SIZE];
  char company[BUFFER_SIZE];
  int numOrdered;
} telegram;

void create_communication();
bool is_client_connection_close(const char* msg);

int main(int argc, char const* argvp[])
{
    create_communication();
}

void create_communication()
{
    telegram tel;
    int client, server;
    char buffer[BUFFER_SIZE];
    bool isExit = false;

    key_t key_company = ftok("key",65);
    key_t key_symbol = ftok("key_symbol_business_logic_shared_memory",66);
    // key_t key_number_ordered = ftok("key_number_ordered_business_logic_shared_memory",65);

    int shm_company = shmget(key_company,1024,0666|IPC_CREAT);
    int shm_symbol = shmget(key_symbol,1024,0666|IPC_CREAT);
    // int shm_number_ordered = shmget(key_number_ordered, 1024, 0666|IPC_CREAT);

    char *str_company = (char*) shmat(shm_company,(void*)0,0);
    char *str_symbol = (char*) shmat(shm_symbol,(void*)0,0);

    server = socket(AF_INET, SOCK_STREAM, 0); // create server socket
    if (server < 0) 
    {
        std::cout << ERROR_S << "Can't accepting client.\n";
        exit(1);
    }
    else {
        std::cout << "[+]TCP server socket created.\n";
    }

    struct sockaddr_in server_address, client_addr;
    socklen_t size = sizeof(server_address);

    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_port = DEFAULT_PORT;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    int ret = bind(server, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address));
    if (ret < 0)
    {
        std::cout << ERROR_S << "binding connection. Socket has already been establishing.\n";
        exit(1); 
    }

    listen(server, 5);
    std::cout << "Listening...\n";

    while(true)
    {
        size = sizeof(size);

        std::cout << "SERVER: " << "Listening clients...\n";  
        client = accept(server, reinterpret_cast<struct sockaddr*>(&client_addr), &size);
        if(client < 0)
        {
            std::cout << "[+]Client not connected.\n";
            exit(1);
        }
        else {
            std::cout << "[+]Client connected.\n";
            bzero(buffer, 1024);  
            recv(client, reinterpret_cast<char*>(&tel), sizeof(tel), 0); 
            // std::cout << "Company: " << tel.company << std::endl;


            strcpy(str_company, tel.company); 
            std::cout << "Data read from memory str_company: " << str_company << std::endl;

            strcpy(str_symbol, tel.symbol);
            std::cout << "Data read from memory str_symbol: " << str_symbol << std::endl;
            

            //destroy the shared memory
            // shmctl(shm_company, IPC_RMID, NULL);
            // shmctl(shm_symbol, IPC_RMID, NULL); 
            // shmctl(shm_number_ordered, IPC_RMID, NULL);
        }
        // break;

    // send(client, (char*)msg, strlen(mem), 0); 
    }
        close(client);
        printf("[+]Client disconnected.\n\n");
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