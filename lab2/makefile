CC = gcc
OBJCLI = ftpc.c 
OBJSRV = ftps.c
CFLAGS = 
# setup for system
LIBS =

all: ftpc ftps 

ftpc:	$(OBJCLI)
	$(CC) $(CFLAGS) -o $@ $(OBJCLI) $(LIBS)

ftps:	$(OBJSRV)
	$(CC) $(CFLAGS) -o $@ $(OBJSRV) $(LIBS)

clean:
	ftpc ftps