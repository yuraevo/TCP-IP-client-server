#include <iostream>
#include <string>
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

#define ERROR_S "SERVER ERROR: "
#define DEFAULT_PORT 1603
#define DEFAULT_PORT_BL 1605
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'
#define BOOST_INTERPROCESS_BOOTSTAMP_IS_SESSION_MANAGER_BASED

std::mutex display_mutex;
using namespace boost::interprocess;

void create_communication_with_client();
void create_communication_with_business_logic();
bool is_client_connection_close(const char* msg);

int main(int argc, char const* argvp[])
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << "ID of process running main program: " << this_id << std::endl;
    std::thread t1(create_communication_with_client);
    t1.join();
}

void create_communication_with_client()
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << "ID of process running communication_with_client program: " << this_id << std::endl;
    display_mutex.lock();

    int client, server, business_logic;
    char buffer[BUFFER_SIZE], buffer_shared_memory[BUFFER_SIZE], opt[10];
    char retData;
    bool isExit = false;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) 
    {
        std::cout << ERROR_S << "Can't accepting client.\n";
        exit(1);
    }
    else {
        std::cout << "[+]TCP server socket created.\n";
    }

    business_logic = socket(AF_INET, SOCK_STREAM, 0);
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

            shared_memory_object::remove("MySharedMemory");
            managed_shared_memory segment(open_read_only, "MySharedMemory");
            managed_shared_memory::handle_t handle = 192; 

            // std::stringstream s;
            // s << 240;
            // s >> handle;

            void *msg = segment.get_address_from_handle(handle);

            // std::cout << "answer for business_logic: \n";
            std::cout << (char*)msg << std::endl;

            // send(client, mem, strlen(mem), 0);
        }
        close(client);
        printf("[+]Client disconnected.\n\n");
        
    }
    display_mutex.unlock();
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