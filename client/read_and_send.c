#include "read_and_send.h"

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

	if(sendpack(sockfd, &filesize, sizeof(filesize), 0) == false)
	{
		perror("send fallo al mandar filesize.");
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

	if(sendpack(sockfd, mapped_file, filesize, 0) == false)
	{
		perror("send fallo.");
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

	if(sendpack(sockfd, &filesize, sizeof(filesize), 0) == false)
	{
		puts("send fallo al mandar filesize.");
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

		if(sendpack(sockfd, buffer, nbytes, 0) == false)
		{
			perror("send fallo.");
			printf("Bytes leidos: %zu\n", total_bytes);
			close(fd);
			return false;
		}
	}

	close(fd);

	duration = clock() - duration;

	*timer = duration;

	return true;
}

#endif
