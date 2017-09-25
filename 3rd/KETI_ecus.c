#include "KETI_Socket.h"
#include "wssdef.h"
#include "KETI_API.h"
#include "KETI_ecus.h"

#define BMS_ORIGIN	"2.1"
#define OMP_ORIGIN	"1.0.2"
#define ABS_ORIGIN	"1.0.0"
#define BCM_ORIGIN	"1.2.0"

#define NUM_OF_ECU	4

/*
char* KETI_virtual_ecus (const char* ecu_name, const char* version)	{

	char return_info [256];
	if (strncmp ("BMS", ecu_name, 3) == 0)	
//		strcpy (return_info, (char*)ECU_BMS(version));
		ECU_BMS(version);
	else if (strncmp ("OMP", ecu_name, 3) == 0)	
		strcpy (return_info, ECU_OMP(version));
	else if (strncmp ("ABS", ecu_name, 3) == 0)	
		strcpy (return_info, ECU_ABS(version));
	else if (strncmp ("BCM", ecu_name, 3) == 0)	
		strcpy (return_info, ECU_BCM(version));
	else	{
		printf ("[KETI ecus] Not found ECU: %s\n", ecu_name);
		return NULL;
	}

	if (return_info == NULL)	{
		printf ("[KETI ecus] failed: %s\n", ecu_name);
	}

	return return_info;
}
*/
 char return_value [256];
	
 char* ECU_BMS (char* new_version)	{
	 static boolean update_flag = 0;
	 static char version [10];
	 const char* dscp = "Battery management system";
	 
	 if (!update_flag && (new_version == NULL)) {
		 strcpy (version, BMS_ORIGIN);
//		sprintf (version, "%s", BMS_ORIGIN);
	 }
	 else if (new_version != NULL)	{
		 update_flag = 1;

		 strcpy (version, new_version);
	 }

	 sprintf (return_value, "%s;%s", version, dscp);

	 return ((char*)return_value);
 }

 char* ECU_OMP (char* new_version)	{
	 static boolean update_flag = 0;
	 static char version [10];
	 const char* dscp = "Main MOST control unit";
	 
	 if (!update_flag && (new_version == NULL)) {
		 strcpy (version, OMP_ORIGIN);
	 }
	 else if (new_version != NULL)	{
		 update_flag = 1;

		 strcpy (version, new_version);
	 }

	 sprintf (return_value, "%s;%s", version, dscp);

	 return ((char*)return_value);
 }

 char* ECU_ABS (char* new_version)	{
	 static boolean update_flag = 0;
	 static char version [10];
	 const char* dscp = "ABS control unit";
	 
	 if (!update_flag && (new_version == NULL)) {
		 strcpy (version, ABS_ORIGIN);
	 }
	 else if (new_version != NULL)	{
		 update_flag = 1;

		 strcpy (version, new_version);
	 }

	 sprintf (return_value, "%s;%s", version, dscp);

	 return ((char*)return_value);
 }

 char* ECU_BCM (char* new_version)	{
	 static boolean update_flag = 0;
	 static char version [10];
	 const char* dscp = "Body control module";
	 
	 if (!update_flag && (new_version == NULL)) {
		 strcpy (version, BCM_ORIGIN);
	 }
	 else if (new_version != NULL)	{
		 update_flag = 1;

		 strcpy (version, new_version);
	 }

	 sprintf (return_value, "%s;%s", version, dscp);

	 return ((char*)return_value);
 }
