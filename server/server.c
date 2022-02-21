#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include "../port.h"
#include "../packet.h"

int main()
{
	int sockfd;
	struct sockaddr_in sockaddr;

	int client_sockfd;
	struct sockaddr_in client_sockaddr;
	socklen_t client_addr_len = sizeof(client_sockaddr);

	char filename[9];
	FILE * stream;
	char buffer[MAX_BUFFER_LEN];
	size_t filesize;
	size_t packsize;
	ssize_t recv_bytes;
	size_t total_bytes = 0;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == 1)
	{
		perror("No se pudo crear la socket.");
		return -1;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(PORT);
	for(char i = 0; i < sizeof(sockaddr.sin_zero)/sizeof(sockaddr.sin_zero[0]); i++)
	{
		sockaddr.sin_zero[i] = 0;
	}

	if(bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
	{
		perror("No se pudo aignar una direccion local a la socket.");
		return -1;
	}

	if(listen(sockfd, 100) == -1)
	{
		perror("No se pudo poner al backlog como 1 y tampoco se pudo marcar a la socket como aceptando conexiones.");
	}

	while(1)
	{
		char op;
		do
		{
			puts("Aceptar conexion? S/N");
			op = getchar();
			for(char ch = op; ch != '\n'; ch = getchar());
		}
		while(op != 'S' && op != 'N');
		if(op == 'N')
		{
			break;
		}

		client_sockfd = accept(sockfd, (struct sockaddr*)&client_sockaddr, &client_addr_len);

		for(size_t i = 0; i < NFILES; i++)
		{
			sprintf(filename, "%d.txt", i);

			stream = fopen(filename, "w");
			if(stream == NULL)
			{
				perror("No se pudo abrir un stream.");
				break;
			}

			printf("%zu.\n",i+1);

			recv_bytes = recv(client_sockfd, &filesize, sizeof(filesize), 0);
			if(recv_bytes == -1)
			{
				perror("No se pudo obtener el size del archivo a recibir: recv fallo.");
				fclose(stream);
				break;
			}
			else if(recv_bytes != sizeof(filesize))
			{
				perror("No se pudo obtener el size del archivo a recibir: paquete incompleto.");
				fclose(stream);
				break;
			}

			packsize = filesize%MAX_PACKET_SIZE;
			if(packsize == 0)
			{
				packsize = MAX_PACKET_SIZE;
			}
			recv_bytes = recv(client_sockfd, buffer, packsize, 0);
			if(recv_bytes == -1)
			{
				perror("No se pudo recibir el primer paquete: recv fallo");
				fclose(stream);
				break;
			}
			else if(recv_bytes != packsize)
			{
				perror("No se pudo recibir el primer paquete: paquete incompleto.");
				fclose(stream);
				break;
			}

			fwrite(buffer, sizeof(char), packsize, stream);
			filesize -= packsize;
			total_bytes = packsize;

			while(filesize != 0)
			{
				recv_bytes = recv(client_sockfd, buffer, MAX_BUFFER_LEN, 0);
				if(recv_bytes == -1)
				{
					perror("No se pudo recibir un paquete: recv fallo");
					break;
				}
				else if(recv_bytes != MAX_BUFFER_LEN)
				{
					perror("No se pudo recibir un paquete: paquete incompleto.");
					break;
				}
				fwrite(buffer, sizeof(char), MAX_BUFFER_LEN, stream);
				filesize -= MAX_BUFFER_LEN;
				total_bytes += MAX_BUFFER_LEN;
			}

			fclose(stream);

			if(filesize != 0)
			{
				perror("No se pudo leer todo el archivo.");
				break;
			}
		}

		printf("Total bytes: %zu\n",total_bytes);

		close(client_sockfd);
	}

	return 0;
}
