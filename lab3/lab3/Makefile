# Makefile for client and server

CC = gcc
OBJCLI = client/client.c 
OBJSRV = server/server.c
OBJCLIT = client/tcpdc.c
OBJSRVT = server/tcpds.c
CFLAGS = 
# setup for system
LIBS =

all: ftpc ftps tcpds tcpdc

ftpc:	$(OBJCLI)
	$(CC) $(CFLAGS) -o client/$@ $(OBJCLI) $(LIBS)

ftps:	$(OBJSRV)
	$(CC) $(CFLAGS) -o server/$@ $(OBJSRV) $(LIBS)

tcpds:	$(OBJSRVT)
	$(CC) $(CFLAGS) -o server/tcpd $(OBJSRVT) $(LIBS)

tcpdc:	$(OBJCLIT)
	$(CC) $(CFLAGS) -o client/tcpd $(OBJCLIT) $(LIBS)

clean:
	rm ftps ftpc tcpds tcpdc