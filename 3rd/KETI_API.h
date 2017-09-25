/****************************************************************************
 *
 * File: KETI_JSON.h
 *
 * $Project: SyncML Client
 *
 * Copyright 2006 WebSync Co, . Ltd. All rights reserved.
 *
 * Unpublished Confidential Information of WebSync Co, . Ltd.  
 * Do Not Disclose.
 *
 *
 ****************************************************************************/
#ifndef _KETI_JSON_H
#define _KETI_JSON_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   unsigned short count;
   unsigned short Reserved;
}COMMON_COUNT_INFO;

typedef struct
{
   uint8 *Name;
   uint8 *Version;
   uint8 *Describe;
   uint8 *FileName;   
   unsigned int FileSize;
   uint8 *Result;
   uint8 *Reserved;
}COMMON_CONTENT_INFO;

void Read_Data(char *text);

void Write_Data(char *text);



#ifdef __cplusplus
}
#endif

#endif
