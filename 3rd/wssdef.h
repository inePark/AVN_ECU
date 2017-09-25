
#ifndef _WSS_DEF_H_
#define _WSS_DEF_H_


// define Default Constant
#ifndef NULL
#define NULL               0
#endif //NULL

#ifndef TRUE
#define TRUE               1
#endif

#ifndef FALSE
#define FALSE              0
#endif


typedef  unsigned long long   uint64;
typedef  long long            int64;

typedef  unsigned char        boolean;     /* Boolean value type. */

typedef  unsigned int         uint32;      /* Unsigned 32 bit value */
typedef  unsigned short       uint16;      /* Unsigned 16 bit value */
typedef  unsigned char        uint8;       /* Unsigned 8  bit value */

typedef  long int             int32;       /* Signed 32 bit value */  
typedef  short                int16;       /* Signed 16 bit value */
typedef  char                 int8;        /* Signed 8  bit value */

typedef  unsigned char        byte;         /* Unsigned 8  bit value type. */
typedef  unsigned short       word;         /* Unsinged 16 bit value type. */
typedef  unsigned long        dword;        /* Unsigned 32 bit value type. */

typedef struct ecu_content	{
	char name [128];
	char version [32];
	char dscp [256];
} ECU;
 
#ifdef __cplusplus
}
#endif          

#endif // _WSS_DEF_H_
