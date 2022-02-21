#include "../packet.h"

#include <sys/socket.h>

#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>

#include <stdio.h>

#include <stdbool.h>

#include <time.h>

#define READ

#ifdef MMAP

bool read_and_send(const char * filename, int sockfd, clock_t * timer)
{
	int fd;
	char * mapped_file;
	struct stat filestat;
	size_t filesize;
	clock_t duration;
	ssize_t sent_bytes;
	size_t total_bytes = 0;

	if(stat(filename, &filestat) == -1)
	{
		perror("stat fallo.");
		return false;
	}
	filesize = filestat.st_size;

	sent_bytes = send(sockfd, &filesize, sizeof(filesize), 0);
	if(sent_bytes == -1)
	{
		perror("send fallo al mandar filesize.");
		return false;
	}
	else if(sent_bytes != sizeof(filesize))
	{
		perror("paquete incompleto al mandar filesize");
		return false;
	}

	duration = clock();

	fd = open(filename, O_RDONLY);
	if(fd == -1)
	{
		perror("No se pudo abrir el archivo.");
		return false;
	}

	mapped_file = mmap(NULL,filesize, PROT_READ, MAP_PRIVATE, fd, 0);
	if(mapped_file == (char*)-1)
	{
		perror("mmap fallo");
		return false;
	}

	sent_bytes = send(sockfd, mapped_file, filesize, 0);
	if(sent_bytes == -1)
	{
		perror("send fallo.");
		munmap(mapped_file,filesize);
		return false;
	}
	else if(sent_bytes != filesize)
	{
		perror("paquete incompleto");
		munmap(mapped_file,filesize);
		return false;
	}

	munmap(mapped_file,filesize);

	close(fd);

	duration = clock() - duration;

	*timer = duration;

	return true;
}

#endif

#ifdef READ

bool read_and_send(const char * filename, int sockfd, clock_t * timer)
{
	int fd;
	char buffer[MAX_BUFFER_LEN];
	struct stat filestat;
	size_t filesize;
	ssize_t nbytes;
	clock_t duration;
	ssize_t sent_bytes;
	size_t total_bytes = 0;

	if(stat(filename, &filestat) == -1)
	{
		puts("stat fallo.");
		return false;
	}
	filesize = filestat.st_size;

	sent_bytes = send(sockfd, &filesize, sizeof(filesize), 0);
	if(sent_bytes == -1)
	{
		puts("send fallo al mandar filesize.");
		return false;
	}
	else if(sent_bytes != sizeof(filesize))
	{
		puts("se mando un paquete incompleto al mandar filesize");
		return false;
	}

	duration = clock();

	fd = open(filename, O_RDONLY);
	if(fd == -1)
	{
		puts("No se pudo abrir el archivo.");
		return false;
	}

	while((nbytes = read(fd, buffer, MAX_BUFFER_LEN)) != 0)
	{
		if(nbytes == -1)
		{
			puts("read fallo.");
			close(fd);
			return false;
		}

		sent_bytes = send(sockfd, buffer, nbytes, 0);
		if(sent_bytes == -1)
		{
			perror("send fallo.");
			printf("Bytes leidos: %zu\n", total_bytes);
			close(fd);
			return false;
		}
		else
		{
			total_bytes += sent_bytes;
			if(sent_bytes != nbytes)
			{
				puts("se mando un paquete incompleto");
				printf("Bytes leidos: %zu\n", total_bytes);
				close(fd);
				return false;
			}
		}
	}

	close(fd);

	duration = clock() - duration;

	*timer = duration;

	return true;
}

#endif
