PRJ_DIR 	= ~/KETI/3rd
EXE_DIR 	=$(PRJ_DIR)
LIB_DIR 	=$(PRJ_DIR)
PROC_DIR 	=$(PRJ_DIR)

#	--------------------------------------------------------------------------
#	Include Directory
#	--------------------------------------------------------------------------
EXE 	= $(PRJ_DIR)/res

#	--------------------------------------------------------------------------
#	Include Directory
#	--------------------------------------------------------------------------
INC 	= -I$(PRJ_DIR) -I./ 

#	--------------------------------------------------------------------------
#	Dynamically loaded library
#	--------------------------------------------------------------------------
LIBS    = -L/lib 

#	--------------------------------------------------------------------------
#	Object Directory
#	--------------------------------------------------------------------------
PROC_OBJ  = $(PROC_DIR)
API_OBJ  = $(LIB_DIR)

#	--------------------------------------------------------------------------
#	PROJECT Object
#	--------------------------------------------------------------------------
API_OBJS   += ${API_OBJ}/main.o

PROC_OBJS   += ${PROC_OBJ}/KETI_API.o
PROC_OBJS   += ${PROC_OBJ}/KETI_Socket.o
PROC_OBJS   += ${PROC_OBJ}/KETI_sim_interf.o
PROC_OBJS   += ${PROC_OBJ}/KETI_ecus.o

OBJS   = $(API_OBJS) $(PROC_OBJS)

#	--------------------------------------------------------------------------
#	Compile Option
#	--------------------------------------------------------------------------
#CC	= /usr/local/arm/2.95.3/bin/arm-linux-gcc 
#CFLAGS  = -g -Wall -O2 $(INC) -lpthread
CC = gcc
#CC = /opt/boundary/Toolchain/ARMHF/gcc-linaro-arm-linux-guneabihf-4.8/bin/arm-linux-guneabihf-gcc

#	--------------------------------------------------------------------------
#	Main Project
#	--------------------------------------------------------------------------
#TARGET	= $(EXE_DIR)/sample
TARGET	= $(EXE_DIR)/KETI_ecu_mng


all		: $(TARGET)
$(TARGET) : $(OBJS)
	@ echo "====================================     LINK "
	$(CC) -o $@ $(OBJS) $(LIBS) -lpthread
#	@ echo "====================================     MAKE STRIP "
#	@ arm-linux-strip $(TARGET)
#	@ echoon

clean : 
	rm -rf *.o KETI_ecu_mng


#	--------------------------------------------------------------------------
#	Proc Module Compile
#	--------------------------------------------------------------------------
$(PROC_OBJ)/KETI_API.o	: $(LIB_DIR)/KETI_API.c 
	$(CC) $(CFLAGS) -c -o $(PROC_OBJ)/KETI_API.o $(LIB_DIR)/KETI_API.c
$(PROC_OBJ)/KETI_Socket.o	: $(LIB_DIR)/KETI_Socket.c 
	$(CC) $(CFLAGS) -c -o $(PROC_OBJ)/KETI_Socket.o $(LIB_DIR)/KETI_Socket.c
$(PROC_OBJ)/KETI_sim_interf.o	: $(LIB_DIR)/KETI_sim_interf.c 
	$(CC) $(CFLAGS) -c -o $(PROC_OBJ)/KETI_sim_interf.o $(LIB_DIR)/KETI_sim_interf.c
$(PROC_OBJ)/KETI_ecus.o	: $(LIB_DIR)/KETI_ecus.c 
	$(CC) $(CFLAGS) -c -o $(PROC_OBJ)/KETI_ecus.o $(LIB_DIR)/KETI_ecus.c

#	--------------------------------------------------------------------------
#	Api Module Compile
#	--------------------------------------------------------------------------
$(API_OBJ)/main.o	: $(LIB_DIR)/engine_main.c
	$(CC) $(CFLAGS) -c -o $(API_OBJ)/main.o $(LIB_DIR)/engine_main.c



