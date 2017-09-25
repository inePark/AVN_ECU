/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
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
#include <time.h>

#include "KETI_API.h"
#include "KETI_Socket.h"

/*
# Get Netwok
Websync-->KETI
   GET_NETWORK
KETI-->Websync
   GET_NETWORK;(상태)
      ex)
      GET_NETWORK;3G
      GET_NETWOK;WIFI
      GET_NETWORK;NONE


# Get ECU Version
Websync-->KETI
   GET_ECUVER
KETI-->Websync
   GET_ECUVER;(ECU count);(ECU Name);(ECU Version);(ECU Descript)
      ex)
      GET_ECUVER;2;ECM;1.0.0;Engine control unit;OMP;1.0.2;Main MOST control unit


# Set ECU Update
Websync-->KETI
   SET_ECUUPDATE;ECM;1.0.1;/DownLoad/DataFile/ECM.data;10240
KETI-->Websync   
   SET_ECUUPDATE;(상태)
      ex)
      SET_ECUUPDATE;ECM;1.0.1;/DownLoad/DataFile/ECM.data;10240;SUCESS
      SET_ECUUPDATE;ECM;1.0.1;/DownLoad/DataFile/ECM.data;10240;FAIL
*/

////////////////////////////////////////////////////////////////////////////////////////////////////


char ret_buffer [512];
char ECU_List [16][32];
boolean Return_EcuInfo (char* file_name, char* version)	{
	char buffer [128];
	FILE* fd;
	char temp [128];
	static unsigned char ECU_list_flag = 0;
	int nLoop;

	
	memset (ret_buffer, 0x00, 512);
	memset (buffer, 0x00, 128);

	if (strcmp (file_name, "Network.dat") == 0)	{

		if ((fd = fopen (file_name, "rt")) == NULL)	{
			printf ("[fopen error] Can't get ECU information\n");
			return FALSE;
		}
		while (!feof(fd))	{
			if (fgets (buffer, 128, fd) == NULL) break;
		   strcat(ret_buffer, buffer);
		}
	
		fclose (fd);

	}	//Network Information

	else if (strcmp (file_name, "ECU_Info.dat") == 0)	{
		if (ECU_list_flag == 0)	{
			ECU_list_flag = 5;

//			strcpy (ECU_List [0], "Updater");
//			strcpy (ECU_List [1], "Accuweather");
			strcpy (ECU_List [0], "MMECU");
			strcpy (ECU_List [1], "SMC");

			strcpy (ECU_List [2], "OMP");
			strcpy (ECU_List [3], "ABS");
			strcpy (ECU_List [4], "BCM");
			strcpy (ECU_List [5], "BMS");

			for (nLoop = 0; nLoop <= ECU_list_flag; nLoop ++)	{
				fd = fopen (ECU_List [nLoop], "w");
				switch (nLoop)	{
//						case 0:
//							fputs ("Updater;1.0.0.1;Universial Software Update System for Automotive", fd);
//							break;
//						case 1:
//							fputs ("Accuweather;1.0.0.1;Accurate Weather Application", fd);
//							break;
						case 0:
							fputs ("MMECU;2.0.1;Multi Media Electronic Control Unit", fd);
							break;
						case 1:
							fputs ("SMC;1401A;SMC is Side Mirror Controller. SMC Controlled side mirror motor and mode", fd);
							break;
			
						case 2:
							fputs ("OMP;1.0.2;Main MOST Control Unit", fd);
							break;
			
						case 3:
							fputs ("ABS;1.0.0;ABS control Unit", fd);
							break;
			
						case 4:
							fputs ("BCM;1.0.2;Body Control Module", fd);
							break;
			
						case 5:
							fputs ("BMS;1.0.2;Battery Management System", fd);
							break;
						default:
							break;
				} //switch

				fclose (fd);

			}	//for

		} // if ECU_list-flag == 0

		strcpy (ret_buffer, "GET_ECUVER;6;");
		//strcat (ret_buffer, );
		//strcat (ret_buffer, ";");	
		
		for (nLoop = 0; nLoop <= ECU_list_flag; nLoop ++)	{
						 
			if ((fd = fopen (ECU_List [nLoop], "rt")) == NULL)	{
				printf ("[fopen error] Can't get ECU information\n");
				return FALSE;
			}
			
			memset (buffer, 0x0, 128);
			while (!feof(fd))	{
				if (fgets (buffer, 128, fd) == NULL) break;
			   strcat(ret_buffer, buffer);
			}

			if (nLoop < ECU_list_flag)
					strcat (ret_buffer, ";");
			fclose (fd);
		} //for
	}	//ECU_Info

	else 	{

		for (nLoop = 0; nLoop <= ECU_list_flag; nLoop ++)	
				if (strcmp (ECU_List [nLoop], file_name) == 0)
						break;
		
		if ((fd = fopen (ECU_List [nLoop], "r+")) == NULL)	{
			printf ("[fopen error] Can't get ECU information\n");
			return FALSE;
		}
		memset (buffer, 0x0, 128);
		while (!feof(fd))	{
			if (fgets (buffer, 128, fd) == NULL) break;
		}
		fclose (fd);
		
		if (buffer != NULL)	{
		strtok (buffer, ";");
		strtok (NULL, ";");
		strcpy (temp, strtok (NULL, ";\n\0"));
//printf ("%s\n", temp);
		}
		else	{
				printf ("ERROR!!!\n");
		}

		fd = fopen (ECU_List [nLoop], "w");
		
		memset (buffer, 0x0, 128);
		strcpy (buffer, file_name);
		strcat (buffer, ";");
		strcat (buffer, version);
		strcat (buffer, ";");
		strcat (buffer, temp);
		fputs (buffer, fd);
		fclose (fd);
		printf ("%s\n", buffer);


		//fd = fopen ("~/KETI/sim_ecu/update", "w+");
		//fclose (fd);
		//system ("mkdir /home/linaro/KETI/sim_ecu/update");
		system ("mkdir ~/avn-plugin/ECUManager/update");
		sleep (2);


	} //ECU Update
	
	
	return TRUE;
}


void Get_Network()
{
   char Network_Info[512]={0,};

   // ============== Get Network Process
   //

   FILE* fd = fopen ("Network.dat", "w+");


   strcat(Network_Info, "GET_NETWORK");
   strcat(Network_Info, ";"); 
   
   strcat(Network_Info, "3G"); //3// NONE / 3G / WIFI
   fputs (Network_Info, fd);
   fclose (fd);
   
   //
   //Get Network Process
    
   
	if (Return_EcuInfo ("Network.dat", 0) == FALSE)	{
		strcpy (ret_buffer, "can't read network state");
	}

   Write_Data(ret_buffer);
}
		

void Get_EcuInfo()
{
//   char ECU_Info[512]={0,};

   // ========================= Get ECU Info Process
   //

	if (Return_EcuInfo ("ECU_Info.dat", 0) == FALSE)	{
		strcpy (ret_buffer, "GET_ECUVER;ERROR");
//	   Write_Data (ret_buffer);
	}
   

   Write_Data (ret_buffer);
}



void Set_EcuUpdate(unsigned char *Ecu_Name, unsigned char *Ecu_Ver, unsigned char *File_Path, unsigned char * File_Size)
{
   char Update_Result[512]={0,};

   printf("Set_EcuUpdate Ecu_Name :%s\n",Ecu_Name);
   printf("Set_EcuUpdate Ecu_Ver :%s\n",Ecu_Ver);
   printf("Set_EcuUpdate File_Path :%s\n",File_Path);
   printf("Set_EcuUpdate File_Size :%s\n",File_Size);

 
   //====================Update Process
   //
   // this section will be corrected later.
   
   strcat(Update_Result, "SET_ECUUPDATE"); 
   strcat(Update_Result, ";"); 

   strcat(Update_Result, Ecu_Name); 
   strcat(Update_Result, ";"); 

   strcat(Update_Result, Ecu_Ver); 
   strcat(Update_Result, ";"); 

   strcat(Update_Result, File_Path); 
   strcat(Update_Result, ";"); 

   strcat(Update_Result, File_Size); 
   strcat(Update_Result, ";"); 
   
   strcat(Update_Result, "SUCCESS"); //result
   
    
   	if (Return_EcuInfo (Ecu_Name, Ecu_Ver) == FALSE)	{
		strcpy (ret_buffer, "SET_ECUUPDATE;ERROR");
	}
   //
   //Update Process
   // =======================================
   
  //  Write_Data (ret_buffer);
   
   
   
     Write_Data(Update_Result);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
char split_command[100]={0,};
char split_data[100][100]={0,};
void split(char *data)
{
   char seps[] = ";\n\0";
   char *token;
   int i=0;

   memset(split_command,0x00,sizeof(split_command));
   memset(split_data,0x00,sizeof(split_data));
   token = strtok( data, seps );
   while( token != NULL )
   {
      if(i==0)
      strcpy(split_command,token);
      else
      strcpy(split_data[i-1],token);

      token = strtok( NULL, seps );
      i++;

   }
}

void Read_Data(char *text)
{
   printf("Read_Data:\n%s\n",text);
   
   split(text);

   if(strncmp (split_command, "GET_NETWORK", 11) == 0)
   {
   Get_Network();
   }
   else if(strncmp (split_command, "GET_ECUVER", 10) == 0)
   {
   Get_EcuInfo();
   }  
   else if(strncmp (split_command, "SET_ECUUPDATE", 13) == 0)
   {
   Set_EcuUpdate(split_data[0],split_data[1],split_data[2],split_data[3]);
   } 
   else {

		  strcat(split_command, ": UNKNOWN COMMAND!");
		  Write_Data (split_command);		   
   }
}


void Write_Data(char *text)
{
   printf("Write_Data:\n%s\n",text);
   
   KETI_Socket_Send(text);  
}
