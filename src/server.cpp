#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <sstream>
#include <stdio.h>
#include <cstdlib> //std::system
#include <mutex>

#define ERROR_S "SERVER ERROR: "
#define DEFAULT_PORT 1603
#define DEFAULT_PORT_BL 1605
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'
#define BOOST_INTERPROCESS_BOOTSTAMP_IS_SESSION_MANAGER_BASED

std::mutex display_mutex;
using namespace boost::interprocess;

void create_communication();
bool is_client_connection_close(const char* msg);

int main(int argc, char const* argvp[])
{
    // At this point, you may notice that the implementation is in test form. Unfortunately, 
    //I stumbled upon the problem described in the comments in the code below. 
    // I've been trying to figure this out for a while. 
    // At the moment, the client, server, and business logic communicate through the process, 
    // and on the business logic side, a shared memory object is created that should be read on the server side 
    // (but not read due to the problem described below) and sent to the client through the process. 
    // As soon as I solve the problem with shared_memory on the server side, 
    // I will add the full implementation of the business logic and update the repository, but for now, please look at what is at the moment.
    create_communication();
}

void create_communication()
{
    int client, server, business_logic;
    char buffer[BUFFER_SIZE], buffer_shared_memory[BUFFER_SIZE], opt[10];
    bool isExit = false;

    server = socket(AF_INET, SOCK_STREAM, 0); // create server socket
    if (server < 0) 
    {
        std::cout << ERROR_S << "Can't accepting client.\n";
        exit(1);
    }
    else {
        std::cout << "[+]TCP server socket created.\n";
    }

    business_logic = socket(AF_INET, SOCK_STREAM, 0); // create business_logic socket
    if (business_logic < 0)
    {
        std::cout << ERROR_S << "Can't accepting business_logic.\n";
        exit(1);
    }
    else {
        std::cout << "[+]TCP business_logic socket created.\n";
    }

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
    std::cout << "Listening...\n";

    while(true)
    {
        size = sizeof(size);
        size_bl = sizeof(size_bl);

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
            recv(client, buffer, sizeof(buffer), 0);
            std::cout << "Client: " << buffer << std::endl;
        }

        business_logic = accept(business_logic, reinterpret_cast<struct sockaddr*>(&business_logic_addr), &size_bl);

        if(business_logic < 0) 
        {
            std::cout << "[+]Business logic not connected.\n";
        }
        else {
            std::cout << "[+]Business logic connected.\n";
            send(business_logic, buffer, strlen(buffer), 0); 
            
            memset(buffer_shared_memory, '\0', sizeof(buffer_shared_memory));
            memset(opt, '\0', sizeof(opt)); 

            // mapped_region region;
            // shared_memory_object shared_memory_object(open_only, "MySharedMemory", read_only);
            // region = mapped_region(shared_memory_object, read_only);
            // void *msg = static_cast<void*>(region.get_address());
            // shared_memory_object::remove("MySharedMemory");

            // !!! Unfortunately, an error pops up in the server binary and so far I can’t figure out what the problem is:
            // libc++abi: terminating with uncaught exception of type boost::interprocess::interprocess_exception: No such file or directory
            managed_shared_memory segment(open_read_only, "MySharedMemory");

            managed_shared_memory::handle_t handle = 192; 
            void *msg = segment.get_address_from_handle(handle);

            std::cout << "Get value from shared memory: " << (char*)msg << std::endl;

            // When the previous error that was described a couple of lines above is solved, 
            // then this is the response of the business logic through the shared memory to the server, 
            // and the server will send it to the client through the process
            // send(client, (char*)msg, strlen(mem), 0); 
        }
        close(client);
        printf("[+]Client disconnected.\n\n");
    }
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