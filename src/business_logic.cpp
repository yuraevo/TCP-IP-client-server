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
#include <stdio.h>

#define ERROR_BL "BL ERROR: "
#define BUFFER_SIZE 1024


int main(int argc, char const* argvp[])
{
   // ftok to generate unique key
    // key_t key = ftok("key_company_business_logic_shared_memory", 65);
    key_t key_company = ftok("key",65);
    key_t key_symbol = ftok("key_symbol_business_logic_shared_memory",66);
    // key_t key_number_ordered = ftok("key_number_ordered_business_logic_shared_memory", 65);

    // shmget returns an identifier in shmid
    // int shmid = shmget(key, 1024, 0666|IPC_CREAT);
    int shm_company = shmget(key_company,1024,0666|IPC_CREAT);
    int shm_symbol = shmget(key_symbol,1024,0666|IPC_CREAT);
    // int shm_number_ordered = shmget(key_number_ordered, 1024, 0666|IPC_EXCL);
  
    // std::map<std::string, int> map_regular_price;
    // map_regular_price["AAPL"] = 100;
    // map_regular_price["IBM"] = 120;
    
    // shmat to attach to shared memory
    // char *str = (char*) shmat(shmid, (void*)0, 0);
    char *str_company = (char*) shmat(shm_company,(void*)0,0);
    char *str_symbol = (char*) shmat(shm_symbol,(void*)0,0);
    // int *int_number_ordered = (int*) shmat(shm_number_ordered, (void*)0, 0);

    // shmdt(int_number_ordered);
    // std::cout << "Data read from memory str_company: "<< str_company << std::endl;
    printf("Data read from memory str_company: %s\n", str_company); 
    printf("Data read from memory str_symbol: %s\n", str_symbol); 
    shmdt(str_company);
    shmdt(str_symbol);
    shmctl(shm_company,IPC_RMID,NULL);
    shmctl(shm_symbol,IPC_RMID,NULL);
    // shmdt(str_symbol);
    // printf("Data read from memory str_company: %s\n", str_company); 
    // printf("Data read from memory str_symbol: %s\n", str_symbol); 
    // printf("Data read from memory int_number_ordered: %s\n", int_number_ordered); 

      
    //detach from shared memory 
    // shmdt(str);
    printf("Disconnected from the server.\n");
    return 0;
}