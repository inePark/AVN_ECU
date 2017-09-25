#include "KETI_sim_interf.h"

char signal_flag;
char get_signal;
unsigned char last_ver;
int fd;
char tx_buf[MAX_NUM];
char rx_buf[MAX_NUM];
char mmecu_flag;
//extern char update_file_path [256];


void signalHandler (int sig_)	{
		if (signal_flag == OFF)
{usleep(8000);	//for faster clock speed of iMX board			
	return ;	}
		else	{
			printf ("\n<<<<<< RECV data from ECU >>>>>>\n");
			recv_from_UCA (stdout);

usleep(8000);	//for faster clock speed of iMX board			
	//		printf ("\nThe last version is 3!\n");
	//		scanf ("%d", &last_ver);
	//		get_signal = ON;
		}
}

int open_port (void)	{
		struct sigaction sa;
		
		bzero (&sa, sizeof(sa));
		sa.sa_handler = signalHandler;
		sa.sa_flags = 0;
		sa.sa_restorer = NULL;
		sigaction (SIGIO, &sa, NULL);

		fd = open ("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);

		if (fd == -1)	{
				//perror("/dev/ttyUSB0");
				return -1;
		}
		else	{
				mmecu_flag = 1;	
			//	fcntl(fd, F_SETFL, 0);
				fcntl (fd, F_SETOWN, getpid());
				fcntl (fd, F_SETFL, FASYNC);
		}
//		printf ("open the port!\n");
		
		return (fd);
}
int set_interface_attr (int speed, int parity)	{
		struct termios tty;
		memset (&tty, 0, sizeof (tty));
		if (tcgetattr (fd, &tty) != 0)	{
				printf ("tcgetattr\n");
				return -1;
		}
//		printf ("get the attribute!\n");

		cfsetospeed (&tty, speed);
		cfsetispeed (&tty, speed);

		tty.c_cflag = (tty.c_cflag & ~CSIZE | CS8);	//8-bit chars

	//	tty.c_iflag &= ~IGNBRK;	//ignore break signal
		tty.c_iflag = (IGNPAR | ICRNL);	//ignore break signal
	//	tty.c_lflag = 0;	//no signaling chars, no echo
		tty.c_lflag = ICANON;	//no signaling chars, no echo
		tty.c_oflag = 0;	//no remapping, no delay
		tty.c_cc[VMIN] = 7;	//read doesn't block
		tty.c_cc[VTIME] = 0;	//0.5 sec read timeout

		tty.c_iflag &= ~(IXON | IXOFF | IXANY);	//shut off xon/xoff ctrl
		tty.c_cflag |= (CLOCAL | CREAD);	//ignore modem controls, enable reading
		tty.c_cflag &= ~CSTOPB;
		tty.c_cflag &= ~CRTSCTS;

		if (tcsetattr (fd, TCSANOW, &tty) != 0)	{
				printf ("tcsetattr\n");
				return -1;
		}
//		printf ("set the attribute!\n");
		return 0;
}

void
set_blocking (int should_block)	{
		struct termios tty;
		memset (&tty, 0, sizeof (tty));
		if (tcgetattr (fd, &tty) != 0)	{
				printf ("tggetattr\n");
				return ;
		}
		tty.c_cc [VMIN] = should_block ? 1 : 0;
		tty.c_cc [VTIME] = 5;

		if (tcsetattr (fd, TCSANOW, &tty) != 0)
				printf ("tcsetattr \n");
}
void print_hex (FILE* file_, char* str, int len)	{
	int temp;
		for (temp = 0; temp < len; temp ++	)	{
			fprintf (file_, "%#X ", str [temp]);
			if (str [temp] == 0xD)
					break;
		}	
		fprintf (file_, "\n");

		return ;
}

void print_string (FILE* file_, char* str, int len)	{
		static unsigned count = 1;
		if (*str == '?')
				printf ("Protocol error!\n");
	/*	else 
			printf ("%d%s\n", count, str);
	*/	fprintf (file_, "%d. %dB[%s]\n", count ++,len,str);
	//		fflush(stdout);
			fflush(file_);

		return ;
}


void* KETI_ECU_simulator (void* par)	{

		FILE* file_out;

		signal_flag = OFF;
		get_signal = OFF;
		last_ver = 0;
		mmecu_flag = 0;

		if (open_port () < 0)	{
			printf ("open_port error!\n");
			exit (1);
			//return ;
		}
		
		mmecu_flag = 1;	

		if (! set_interface_attr (B115200, 0))
			set_blocking (0);
		
		size_t wn, n  = 0;

		file_out = fopen("debug", "w+");
/*
		if ((tx_buf = (char*) malloc (MAX_NUM * sizeof(char))) == NULL)	{
			printf ("TX MEM error!\n");
			exit (-1);
		}
		if ((rx_buf = (char*) malloc (MAX_NUM * sizeof(char))) == NULL)	{
			printf ("RX MEM error!\n");
			exit (-1);
		}
*/

		/* Env. setting read order */
		env_set_read (file_out);
		
		usleep(5000);
		recv_from_UCA (file_out);

		/* CAN RX start order */
		can_rx_start (file_out);

		usleep (5000);
		recv_from_UCA (file_out);

		fclose (file_out);
	
		while (1)	{
				file_out = fopen("debug", "a+");
//usleep(500000);
				sleep(1);
				n = user_interface_ ();	
				if (can_tx_data (n) == -1 ) 
					break;
		
			//signal_flag = OFF;
				wn = write (fd, tx_buf, 27);		//TX
				print_hex (file_out, tx_buf, 27);
			//fflush((FILE*)fd);
			//signal_flag = ON;

			//usleep (10000);
				recv_from_UCA (file_out);
			
				if (n == 2)	{
						print_ECU_ver (n);
				}

				fclose (file_out);
		}

		close (fd);
/*		free (tx_buf);
		free (rx_buf);
*/
		return 0;
}

void print_ECU_ver	(int n)	{
		static char temp_ver = 0;
		/*char temp_string [MAX_NUM];
		char* address_;
		strcpy (temp_string, rx_buf);
		strtok (temp_string, ":");
		address_ = strtok (NULL, ":");
		printf ("** ECU VERSION: %c\n\n", *(address_ +17 ) );
		*/
		printf ("** ECU VERSION: %d\n\n", temp_ver+1);
		temp_ver = 1;

}



void env_set_read (FILE* file_out)	{

		//signal_flag = OFF;

		memset(tx_buf, 0, MAX_NUM);
		tx_buf [0] = ':';	//start
		tx_buf [1] = 'Y';
		tx_buf [2] = 0x35;	//3
		tx_buf [3] = 0x39;	//9
		tx_buf [4] = 0x0D;	//end

		write (fd, tx_buf, 5);	//TX
		print_hex (file_out, tx_buf, 5);

		//signal_flag = ON;

}


int recv_from_UCA (FILE* file_out)	{

		//signal_flag = OFF;

		memset(rx_buf, 0, MAX_NUM);
		int n = read (fd, rx_buf, MAX_NUM);	//RX
		print_string (file_out, rx_buf, n);
		print_hex (file_out, rx_buf, n);

		//signal_flag = ON;
		return 0;
}

void can_rx_start (FILE* file_out)	{
		
		//signal_flag = OFF;

		memset(tx_buf, 0, MAX_NUM);
		tx_buf [0] = ':';	//start
		tx_buf [1] = 'G';
		tx_buf [2] = 0x31;	//1
		tx_buf [3] = 0x31;	//1
		tx_buf [4] = 0x41;	//chk sum1: A
		tx_buf [5] = 0x39;	//chk sum2: 9
		tx_buf [6] = 0xD;	//end

		write (fd, tx_buf, 7);		//TX
		print_hex (file_out, tx_buf, 7);
		
		//signal_flag = ON;
}

int user_interface_ ()	{

		signal_flag = ON;
		FILE* update_check;
		DIR* dir_;
		char temp_buf [256];
		char user_path [256];

		printf ("==== Waiting for NEW update file ===\n");

			while ((update_check = fopen ("update", "r")) == NULL)	{
				sleep(1);
			//	printf ("1"); fflush(stdout);
			}

			sleep (2);
			fclose (update_check);
			//system ("rmdir ~/avn-plugin/ECUManager/update");

		signal_flag = OFF;
		return 4;

}

unsigned char check_sum()	{

	char temp = 0; 

	int i;
	for (i = 1; i < 26; i ++)	{
		temp += tx_buf [i];
//		printf ("%2X += %2X \n", temp, tx_buf [i]);
	}

	temp &= 0xFF;
	return temp;
}


char hex2ascii (char in)	{

		if (in == 'A' || in == 'a' || in == 10)	return 0x41;	
		else if (in == 'B' || in == 'b' || in == 11)	return 0x42;	
		else if (in == 'C' || in == 'c' || in == 12)	return 0x43;	
		else if (in == 'D' || in == 'd' || in == 13)	return 0x44;	
		else if (in == 'E' || in == 'e' || in == 14)	return 0x45;	
		else if (in == 'F' || in == 'f' || in == 15)	return 0x46;
		else
			return (in + 0x30);
}

int can_tx_data	(int order_)	{

	int size_, a;
	static unsigned counter = 1;
	unsigned t;
	char * trx;

	char chksum = 0;
	unsigned char buffer [MAX_NUM];

	
	FILE* fd_update = fopen ("update", "r");
	fgets (buffer, 32, fd_update);
	fclose (fd_update);
	memset(tx_buf, 0, MAX_NUM);
	tx_buf [0] = ':';	//start
	tx_buf [1] = 'W';
	tx_buf [2] = 0x30;	//0
	tx_buf [3] = 0x38;	//8
	tx_buf [4] = 0x30;	//tx ID1: 0
	tx_buf [5] = 0x36;	//tx ID2: 6
	tx_buf [6] = 0x46;	//tx ID3: F
//	if (update_file_path [0] == 'U')
	if (buffer [0] == 'U')
		tx_buf [7] = 0x45;	//tx ID4: E
	else if (buffer [0] == 'D')	
		tx_buf [7] = 0x46;	//tx ID4: F
	

	//system ("rm update");
	if (mmecu_flag && (remove ("update") == 0))	{
		usleep (5000);
		printf ("Update DONE!!\n\n");
		fd_update = fopen ("okay", "w+");
		fclose (fd_update);
	}
	else 
		printf ("update error!!\n\n");
	
	
/*	unsigned loop;
	FILE* sendfile = fopen ("asdf", "rb");
	memset (buffer, 0x00, 32);
	fread (buffer, 1, 16, sendfile);

	strcat (tx_buf, buffer);
	if ((trx = strchr (tx_buf, 0x0)) < (tx_buf + 24))	{
		for (loop = trx - tx_buf; loop < 24; loop ++)	{
			tx_buf [loop] = 0x30;
		}
	}
*/
//	if ((trx = strchr (tx_buf, 0x0)) < (tx_buf + 24))	{
	int loop;
		for (loop =8; loop < 24; loop ++)	{
			tx_buf [loop] = 0x30;
		}
//	}

	chksum = check_sum ();
	tx_buf [24]= chksum & 0xF0;
	tx_buf [24]= tx_buf [24] >> 4;
	tx_buf [24] &= 0x0F;
	tx_buf [25]= chksum & 0x0F;
	/*
	t = 0;
	trx = (tx_buf + 24);
	sscanf (trx, "%c",&t);
//	sprintf (&tx_buf [24], "%2X", t);
	printf ("/%d/\n", tx_buf [24]);
//	tx_buf [24] = (unsigned char)t;
	trx = (tx_buf + 25);
	sscanf (trx, "%d", &t);
	tx_buf [25] = (unsigned char)t;
	*/

	tx_buf [24] = hex2ascii (tx_buf [24]);
	tx_buf [25] = hex2ascii (tx_buf [25]);

	tx_buf [26] = 0x0D;
/*	
	for (a = 0; a < 27; a ++)	{
	printf ("[%d] tx_buf [%d]: %2X\n", counter, a, tx_buf [a]);
	}
*/

	counter ++;
	return 27;



}

