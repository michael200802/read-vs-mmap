#pragma once

#define NFILES 100

#define MAX_BUFFER_LEN 100

#define MAX_PACKET_SIZE MAX_BUFFER_LEN

#include <stdbool.h>
#include <sys/socket.h>
#include <errno.h>

bool sendpack(const int sockfd, void * _packet, size_t nbytes, const int flags);

bool recvpack(const int sockfd, void * _packet, size_t nbytes, const int flags);
