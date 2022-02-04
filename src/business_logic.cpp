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
#include <sys/msg.h>

#define ERROR_BL "BL ERROR: "
#define BUFFER_SIZE 1024

int main(int argc, char const* argvp[])
{
    key_t key_symbol = ftok("key_symbol",65);
    key_t key_company = ftok("key_company",65);
    // key_t key_number_ordered = ftok("key_number_ordered", 65);

    int shm_symbol = shmget(key_symbol,1024,0666);
    int shm_company = shmget(key_company,1024,0666);
    // int shm_number_ordered = shmget(key_number_ordered, 1024, 0666|IPC_EXCL);
  
    // std::map<std::string, int> map_regular_price;
    // map_regular_price["AAPL"] = 100;
    // map_regular_price["IBM"] = 120;

    char *str_symbol = (char*) shmat(shm_symbol,(void*)0,0);
    char *str_company = (char*) shmat(shm_company,(void*)0,0);
    
    // int *int_number_ordered = (int*) shmat(shm_number_ordered, (void*)0, 0);

    printf("Data read from memory str_symbol: %s\n", str_symbol);  
    shmdt(str_symbol);
    printf("Data read from memory str_company: %s\n", str_company); 
    shmdt(str_symbol);

    shmctl(shm_company,IPC_RMID,NULL);
    shmctl(shm_symbol,IPC_RMID,NULL); 

    
  
    // msgsnd to send message
    // msgsnd(msgid, &m_telegram, sizeof(m_telegram), 0);
  
    // display the message
    // printf("Data send is : %s \n", message.mesg_text);

    printf("Disconnected from the server.\n");
    return 0;
}