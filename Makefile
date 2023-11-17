CC = gcc
FLAGS = -Wall -std=c99
DEPENDENCIES = -lws2_32
SERVERSRC = tcpserver.c
CLIENTSRC = tcpclient.c

SERVEREXE = server.exe
CLIENTEXE = client.exe

all: $(SERVEREXE) $(CLIENTEXE)

$(SERVEREXE): $(SERVERSRC)
	$(CC) $(FLAGS) $(SERVERSRC) -o $(SERVEREXE) $(DEPENDENCIES)

$(CLIENTEXE): $(CLIENTSRC)
	$(CC) $(FLAGS) $(CLIENTSRC) -o $(CLIENTEXE) $(DEPENDENCIES)

clean:
	rm -f $(SERVEREXE) $(CLIENTEXE)