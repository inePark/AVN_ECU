/*
* MagicSync
* Copyright (c) 2006 WEBSYNC.
* All right reserved.
*
*/

#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "KETI_Socket.h"
#include "KETI_sim_interf.h"

char update_file_path [256];

ECU ECUs [20];

int main()
{

	char  s[100];
	char  t[100];   
//   KETI_Socket_Service();
	
	int pid;
	FILE* fd_main;

	pthread_t	thread_vdm;
	pthread_t	thread_sim;

	
	memset (ECUs, 0x0, sizeof (ECU) * 20);
	memset (update_file_path, 0x0, 256);

	if ((fd_main = fopen ("update", "r")) != NULL )	{
		fclose (fd_main);
		remove ("update");
	}
	
	if ((fd_main = fopen ("okay", "r")) != NULL )	{
		fclose (fd_main);
		remove ("okay");
	}
	
	pid = fork();

	if (pid > 0)	{

		if (pthread_create (&thread_vdm, NULL, KETI_Socket_Service, (void *)NULL) == -1)
		{
			printf ("[KETI_VDM] pthread creation error: VDM\n");
			exit(1);
		}
	
	}
	else 	{
		sleep(1);
		if (pthread_create (&thread_sim, NULL, KETI_ECU_simulator, (void *)NULL) == -1)	
		{
			printf ("[KETI_VDM] pthread creation error: SIM\n");
			exit(1);
		}
	}


   while(1)
   {
   	sleep(5);
   }
}

