#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <cstdlib> //std::system
#include <mutex>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <map>
#include <string>

#define ERROR_BL "BL ERROR: "
#define BUFFER_SIZE 1024


int main(int argc, char const* argvp[])
{
   // ftok to generate unique key
    key_t key = ftok("business_logic_shared_memory", 65);

    // shmget returns an identifier in shmid
    int shmid = shmget(key, 1024, 0666|IPC_CREAT);
  
    std::map<std::string, int> map_regular_price;
    map_regular_price["AAPL"] = 100;
    map_regular_price["IBM"] = 120;
    
    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid, (void*)0, 0);
  
    std::cout<<"Write Data : ";
    gets(str);
  
    printf("Data written in memory: %s\n", str);
      
    //detach from shared memory 
    shmdt(str);

    printf("Disconnected from the server.\n");
    return 0;
}