#include "packet.h"

bool sendpack(const int sockfd, void * _packet, size_t nbytes, const int flags)
{
        char * packet = _packet;
        ssize_t sent_bytes;

        while(nbytes != 0)
        {
                sent_bytes = send(sockfd, packet, nbytes, flags);
                if(sent_bytes == -1)
                {
                        if(errno != EINTR)
                        {
                	        return false;
                        }
                }
                else
                {
                        packet += sent_bytes;
                        nbytes -= sent_bytes;
                }
        }

        return true;
}

bool recvpack(const int sockfd, void * _packet, size_t nbytes, const int flags)
{
        char * packet = _packet;
        ssize_t recv_bytes;

        while(nbytes != 0)
        {
                recv_bytes = recv(sockfd, packet, nbytes, flags);
                if(recv_bytes == -1)
                {
                        if(errno != EINTR)
                        {
                                return false;
                        }
                }
                else
                {
                        packet += recv_bytes;
                        nbytes -= recv_bytes;
                }
        }

        return true;
}
