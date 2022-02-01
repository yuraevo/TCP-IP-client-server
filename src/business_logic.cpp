#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cstdlib> //std::system
#include <mutex>

#define ERROR_BL "BL ERROR: "
#define DEFAULT_PORT 1605
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'

using namespace boost::interprocess;

bool is_client_connection_close(const char* msg);

int main(int argc, char const* argvp[])
{
    int business_logic = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[BUFFER_SIZE], buffer_shared_memory[BUFFER_SIZE];

    if (business_logic < 0)
    {
        std::cout << ERROR_BL << "establishing socket error.";
        exit(1);
    }
    else {
        std::cout << "\n=> BusinessLogic socket created\n";
    }

    struct sockaddr_in server_address;
    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    int ret = connect(business_logic, 
        reinterpret_cast<const struct sockaddr*>(&server_address),
        sizeof(server_address));

    if(ret == 0)
    {
        std::cout << "=> Connection to server " 
        << inet_ntoa(server_address.sin_addr) 
        << "with port number: " << DEFAULT_PORT << "\n";
    }

    bzero(buffer, BUFFER_SIZE);
    recv(business_logic, buffer, sizeof(buffer), 0);
    printf("Getting a value from the server: %s\n", buffer);

    struct shm_remove
    {
        shm_remove() {shared_memory_object::remove("MySharedMemory");}
        ~shm_remove() {shared_memory_object::remove("MySharedMemory");} 
    } remover;

    managed_shared_memory segment (create_only, "MySharedMemory", 65536); // create managed_shared_memory member 
    managed_shared_memory::size_type free_memory_before = segment.get_free_memory();
    void *shptr = segment.allocate(1024);
    managed_shared_memory::size_type free_memory_after = segment.get_free_memory();

    std::cout << "Free memory before: " << free_memory_before << std::endl;
    std::cout << "Free memory after: " << free_memory_after << std::endl;
    std::cout << "Free memory difference: " << free_memory_before - free_memory_after << std::endl;

    managed_shared_memory::handle_t handle = segment.get_handle_from_address(shptr);
    std::cout << "Shared Memory handle: " << handle << std::endl;

    memset(shptr, '\0', 1024);
    memcpy((char*)shptr, buffer, strlen(buffer)); // so far, the usual test copy in order to display it on the server using shared memory
    std::cout << "Shared memory value: " << (char*)shptr << std::endl;   

    close(business_logic);
    printf("Disconnected from the server.\n");
    shared_memory_object::remove("MySharedMemory"); 
    return 0;
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