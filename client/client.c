#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../port.h"
#include "../packet.h"
#include "read_and_send.h"
#include <time.h>

int main()
{
	int sockfd;
	struct sockaddr_in sockaddr;

	char filename[9];

	clock_t average_duration = 0, new_duration;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1",&sockaddr.sin_addr.s_addr);
	sockaddr.sin_port = htons(PORT);
	for(char i = 0; i < 8; i++)
	{
		sockaddr.sin_zero[i] = 0;
	}

	if(connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) != -1)
	{
		for(size_t i = 0; i < NFILES; i++)
		{
			sprintf(filename,"%d.txt",i);

			puts(filename);

			if(read_and_send(filename, sockfd, &new_duration) == false)
			{

				printf("Error: No se pudo enviar el archivo %zu.\n",i);

				break;
			}

			average_duration += new_duration;
		}
	}
	else
	{
		perror("No se pudo establecer una conexion.");
	}

	printf("Average duration: %Lf\n",(long double)(average_duration/NFILES));

	return 0;
}
