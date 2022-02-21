compiler=gcc

all: client/client server/server

client/client: client/client.c client/read_and_send.h port.h packet.h
	$(compiler) client/client.c -o $@

server/server: server/server.c port.h packet.h
	$(compiler) server/server.c -o $@


