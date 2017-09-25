

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/signal.h>

#define MAX_NUM 128
#define OFF	0
#define ON	1
int open_port ();
int set_interface_attr (int, int);
void set_blocking (int);
void print_hex	(FILE*, char*, int);
void print_string (FILE*, char*, int);
void env_set_read (FILE*);
int recv_from_UCA (FILE*);
void can_rx_start (FILE*);
int can_tx_data (int);
void print_ECU_ver (int);
int user_interface_ ();
void* KETI_ECU_simulator (void*);
