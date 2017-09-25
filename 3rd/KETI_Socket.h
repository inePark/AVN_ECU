/****************************************************************************
 *
 * File: KETI_Socket.h
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
#ifndef _KETI_SOCKET_H_
#define _KETI_SOCKET_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "wssdef.h"


void* KETI_Socket_Service(void* par);
int KETI_Socket_Send(uint8 *buffer);

#ifdef __cplusplus
}
#endif

#endif

