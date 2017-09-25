/*
* MagicSync
* Copyright (c) 2006 WEBSYNC.
* All right reserved.
*
*/
#include "wssdef.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <syslog.h>
#include <assert.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <unistd.h>

#include <dirent.h>
#include <net/if.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>

  
#include "KETI_Socket.h"
#include "KETI_API.h"

#define  SOCKET_PORT          "8888"
#define FOR_TEST	1
//#undef FOR_TEST	
static int server_id = -1; 
static int client_id = -1; 

pthread_mutex_t _data_mutex;


int KETI_Socket_Close(uint8 *buffer)
{
   printf("[KETI_Socket_Send] socket close \n");
   close(client_id);   
   return TRUE;
}


int KETI_Socket_Send(uint8 *buffer)
{
   if( send(client_id, buffer, strlen(buffer),0) < 0 ) 
   {
      printf("[KETI_Socket_Send] send error\n");
      close(client_id);
      return FALSE;
   }
   return TRUE;
}

void KETI_Socket_Receive()
{
   struct pollfd  tPfds;
   uint8     buffer[1024];   
   uint8     data[1024];      
   int32    nRet;
   int      nReceiveLen;

   printf("KETI_Socket_Receive start\n");

   memset(buffer, 0x00, 1024);
   memset(data, 0x00, 1024);

   tPfds.fd = client_id;
   tPfds.events = POLLIN|POLLPRI|POLLERR|POLLHUP ;

   pthread_mutex_init(&_data_mutex, NULL);
 
 
  
   
   while(1)
   {
      nRet = poll(&tPfds,1,-1);      
      if(nRet==-1)
      {
         printf("[KETI_Socket_Receive] poll error\n"); 
      }
      else if(nRet==0)
      {      
         printf("[KETI_Socket_Receive] poll timeout\n"); 
      }
      else if(tPfds.revents & POLLHUP )
      {
         printf("[KETI_Socket_Receive] poll POLLHUP\n");
         break;
      }
      else if(tPfds.revents & POLLIN )
      {
         while(1)
         {      
			memset(buffer, 0x00, 1024);
            nReceiveLen = recv(client_id, buffer,1024,MSG_DONTWAIT);
            if(nReceiveLen <= 0)
            {
               if(strlen(data) > 0)
               {
               Read_Data(data);
               memset(data,0x00, 1024);               
               }
               break;
            }
            else
            {
               pthread_mutex_lock(&_data_mutex);              
               strcpy(data, buffer);
               pthread_mutex_unlock(&_data_mutex);                  
            }
         }
      }
      else 
      {
         printf("[KETI_Socket_Receive] poll revents:%x\n",tPfds.revents); 
      }
   }
}


void* KETI_Socket_Service(void *par)
{	
	pthread_t thread;

   struct sockaddr_in client_address;   

   if( (client_id = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
   {
      printf("[KETI_Socket_Send_Service] socket error\n");
      return FALSE;
   }
   else
   {
      printf("[KETI_Socket_Send_Service] socket create client_id %d\n", client_id);
   }

   memset(&client_address,0x00, sizeof(struct sockaddr_in));
   client_address.sin_family = AF_INET;
   client_address.sin_addr.s_addr = INADDR_ANY;
   client_address.sin_port = htons(atoi(SOCKET_PORT));  
   
   if( (server_id=connect(client_id,(struct sockaddr*)&client_address,sizeof(client_address))) != 0  ) 
   {
      printf("[KETI_Socket_Send_Service] connect error\n");
      close(client_id);
      return FALSE;
   }

   printf("[KETI_Socket_Service] connect server_id: %d client_id: %d \n",server_id, client_id);

#ifdef FOR_TEST 
sleep(3);
//Read_Data ("GET_NETWORK");
//sleep(5);
Read_Data ("GET_ECUVER");
//sleep(5);
//Read_Data ("SET_ECUUPDATE;BMS;3.0.0;/download/datafile;525");
sleep(5);

Read_Data ("SET_ECUUPDATE;MMECU;2.0.3;/download/datafile;1025");
sleep(5);
Read_Data ("SET_ECUUPDATE;MMECU;2.0.1;/download/datafile;1025");
sleep(5);
Read_Data ("GET_ECUVER");

#else 
	KETI_Socket_Receive ();
#endif
}


