compiler=gcc

all: client/client server/server client/fill

client/client: client/client.o client/read_and_send.o packet.o
	$(compiler) $? -o $@

client/client.o: client/client.c port.h packet.h
	$(compiler) -c client/client.c -o $@

client/read_and_send.o: client/read_and_send.c client/read_and_send.h packet.h
	$(compiler) -c client/read_and_send.c -o $@

server/server: server/server.c port.h packet.h packet.o
	$(compiler) server/server.c packet.o -o $@

client/fill: client/fill.c packet.h
	$(compiler) client/fill.c -o $@

packet.o: packet.h packet.c
	$(compiler) -c packet.c -o $@
