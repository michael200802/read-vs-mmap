#include "../packet.h"

#include <sys/socket.h>

#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>

#include <stdio.h>

#include <stdbool.h>

#include <time.h>

#include <errno.h>

#define MMAP

bool read_and_send(const char * filename, int sockfd, clock_t * timer);
