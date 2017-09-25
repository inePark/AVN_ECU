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
#include "KETI_ecus.h"
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
//char network_state [512];
char ECU_List [128][32];
char split_command[100]={0,};
char split_data[100][100]={0,};
unsigned char update_flag_jmp = 0;
unsigned char ECU_list_flag = 0;


extern return_value [256];
extern ECU ECUs [20];
extern char update_file_path [256];


boolean get_ecu_default_info()	{

	char temp_ecu_info [256];
	ECU_list_flag = 4;

	strcpy(temp_ecu_info, ECU_BMS(NULL));
	if (temp_ecu_info == NULL)	{
		printf ("[KETI API] getting BMS info. failed\n");
		return 0;
	}
	else	{ 
		strcpy (ECUs[0].name, "BMS");
		strcpy (ECUs[0].version, strtok (temp_ecu_info, ";"));
		strcpy (ECUs[0].dscp, strtok (NULL, "\0\n;"));
//		printf ("%s/%s/%s\n", ECUs[0].name, ECUs[0].version, ECUs[0].dscp);
	}

	strcpy(temp_ecu_info, ECU_OMP(NULL));
	if (temp_ecu_info == NULL)	{
		printf ("[KETI API] getting OMP info. failed\n");
		return 0;
	}
	else	{ 
		strcpy (ECUs[1].name, "OMP");
		strcpy (ECUs[1].version, strtok (temp_ecu_info, ";"));
		strcpy (ECUs[1].dscp, strtok (NULL, "\0\n;"));
//		printf ("%s/%s/%s\n", ECUs[0].name, ECUs[0].version, ECUs[0].dscp);
	}
	strcpy(temp_ecu_info, ECU_ABS(NULL));
	if (temp_ecu_info == NULL)	{
		printf ("[KETI API] getting ABS info. failed\n");
		return 0;
	}
	else	{ 
		strcpy (ECUs[2].name, "ABS");
		strcpy (ECUs[2].version, strtok (temp_ecu_info, ";"));
		strcpy (ECUs[2].dscp, strtok (NULL, "\0\n;"));
//		printf ("%s/%s/%s\n", ECUs[0].name, ECUs[0].version, ECUs[0].dscp);
	}
	strcpy(temp_ecu_info, ECU_BCM(NULL));
	if (temp_ecu_info == NULL)	{
		printf ("[KETI API] getting BCM info. failed\n");
		return 0;
	}
	else	{ 
		strcpy (ECUs[3].name, "BCM");
		strcpy (ECUs[3].version, strtok (temp_ecu_info, ";"));
		strcpy (ECUs[3].dscp, strtok (NULL, "\0\n;"));
//		printf ("%s/%s/%s\n", ECUs[0].name, ECUs[0].version, ECUs[0].dscp);
	}

	//////temp
	strcpy (ECUs[4].name, "MMECU");
	strcpy (ECUs[4].version, "2.0.1");
	strcpy (ECUs[4].dscp, "Multimedia electronic control unit");


	return 1;

}





boolean Return_EcuInfo (char* file_name, char* version)	{
	char buffer [128];
	FILE* fd;
	char ecu_num [4];
	char temp [128];
	unsigned nLoop;
	char temp_ver [100];

	strcpy (temp_ver, version); 

	memset (ret_buffer, 0x00, 512);
	memset (buffer, 0x00, 128);

	//if ((strcmp (split_command, "SET_ECUUPDATE") != 0) &&(strcmp (version, "88") != 0))	

		
	char temp_version [32] = {0, };

	for (nLoop = 0; nLoop <= ECU_list_flag; nLoop ++)		{
		if (strcmp (ECUs[nLoop].name, file_name) == 0)	{

			strcpy (temp_version, ECUs[nLoop].version);
			if (strcmp (ECUs[nLoop].version, version) == 0)	{
				printf ("[KETI mmecu] Recv same version. Origin: %s, Recv: %s\n", ECUs[nLoop].version, version);
				nLoop = ECU_list_flag + 1;
				break;
			}

			if (strcmp	(file_name, "MMECU") == 0)	{
				fd = fopen ("update", "w");

				if (strcmp (temp_version, version) < 0)	{	//now, get higher version
				
				printf ("[KETI mmecu] Version UP! Origin: %s, Recv: %s\n", ECUs[nLoop].version, version);
	
			//update_file_path [0] = 'U';
			//strcat (update_file_path, split_data [2]);
					fprintf( fd, "U\n\0");
				}	

				else	{		//I already got higher version
		
			//update_file_path [0] = 'D';
			//strcat (update_file_path, split_data [2]);
				printf ("[KETI mmecu] Version DOWN! Origin: %s, Recv: %s\n", ECUs[nLoop].version, version);
					fprintf( fd, "D\n\0");

				}
				fclose (fd);

				char timer_ = 0;	
				while ((fd = fopen ("okay", "r")) == NULL)	{
					sleep(1);
					timer_ ++;
					if (timer_ > 5)
						return FALSE;
				}
				usleep (1000);
				fclose (fd);
				remove ("okay");

				strcpy (ECUs[nLoop].version, version);
				break;
			}	//if (MMECU)


			switch (nLoop)	{
				strcpy (ECUs[nLoop].version, version);
				case 0:	
					if (ECU_BMS(version) == NULL)
						printf ("[KETI API] UPDATE FAILED\n");
					break;
				case 1:	
					if (ECU_OMP(version) == NULL)
						printf ("[KETI API] UPDATE FAILED\n");
					break;
				case 2:	
					if (ECU_ABS(version) == NULL)
						printf ("[KETI API] UPDATE FAILED\n");
					break;
				case 3:	
					if (ECU_BCM(version) == NULL)
						printf ("[KETI API] UPDATE FAILED\n");
					break;
			}
			break;
		}
	}

	if (nLoop > ECU_list_flag)
		return FALSE;
		
		//memset (buffer, 0x0, 128);	

		//fd = fopen ("~/KETI/sim_ecu/update", "w+");

	 //ECU Update

	return TRUE;
}


void Get_Network()
{

   // ============== Get Network Process
   //

	char Network_Info [128];
	memset (Network_Info, 0x0, 128);

   strcat(Network_Info, "GET_NETWORK");
   strcat(Network_Info, ";"); 
   // NONE / 3G / WIFI


	char command_ [2048] = {0, };
	char temp_ [256];
	char *value_  = "S";
	system ("ifconfig > output");

	FILE* input;
	input = fopen ("output", "r+");
	fread (command_, 1, 2048, input);
	fclose (input);

	strcpy (temp_, strtok(command_, "\n\0"));

	while (value_ != NULL)	{
		strcpy (temp_, value_);
		value_ =  strtok (NULL, "\n\t ");

		if (strcmp (temp_, "wlan1") == 0)	{
   			strcat(Network_Info, "WIFI");
			break;
		}
		if (strcmp (temp_, "ppp0") == 0)	{
   			strcat(Network_Info, "3G");
			break;
		}

	}
	if (value_ == NULL)
   		strcat(Network_Info, "NONE");   

	system ("rm output");

   Write_Data(Network_Info);
}
		

void Get_EcuInfo()
{
	
	char ecu_num [10];
	strcpy (ret_buffer, "GET_ECUVER;");
	sprintf (ecu_num, "%d", ECU_list_flag+1);
	strcat (ret_buffer, ecu_num);

	int nLoop;
	for (nLoop = 0; nLoop <= ECU_list_flag; nLoop ++)	{
		
		strcat (ret_buffer, ";");
		strcat (ret_buffer, ECUs [nLoop].name);
		strcat (ret_buffer, ";");
		strcat (ret_buffer, ECUs [nLoop].version);
		strcat (ret_buffer, ";");
		strcat (ret_buffer, ECUs [nLoop].dscp);
	}

//	if (Return_EcuInfo ("ECU_Info.dat", "1") == FALSE)	{
//		strcpy (ret_buffer, "GET_ECUVER;ERROR");
//	}
   

   Write_Data (ret_buffer);
}



void Set_EcuUpdate(unsigned char *Ecu_Name, unsigned char *Ecu_Ver, unsigned char *File_Path, unsigned char * File_Size)
{
   char Update_Result[512]={0,};

   printf("Set_EcuUpdate Ecu_Name :%s\n",Ecu_Name);
   printf("Set_EcuUpdate Ecu_Ver :%s\n",Ecu_Ver);
   printf("Set_EcuUpdate File_Path :%s\n",File_Path);
   printf("Set_EcuUpdate File_Size :%s\n",File_Size);
   
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
 
   	if (Return_EcuInfo (Ecu_Name, Ecu_Ver) == FALSE)	{
		strcat (Update_Result, "FAIL");	
	}
   //====================Update Process
   //
   // this section will be corrected later.
   
   else	{
   
	   strcat(Update_Result, "SUCCESS"); //result
   		
		if (strcmp (Ecu_Name, "MMECU") == 0)
			while (update_file_path [0] != 0x0)	
				usleep(100);
   }
    
   //
   //Update Process
   // =======================================
 
 	 
     Write_Data(Update_Result);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
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
	static boolean first_exc = 1;
   printf("Read_Data:\n%s\n",text);
   
   if (first_exc) 	{
	   if (!get_ecu_default_info())	{
		   printf ("[KETI API] can't get ecu's info.\n");\
		   return ;
		}

	   first_exc = 0;
   }


	char temp[1024];
	strcpy(temp, text);
	int nLoop;
   split(temp);

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



