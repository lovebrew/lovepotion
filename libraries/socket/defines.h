#pragma once

#define SO_SOCKNAME_ALL "*"
#define SOCKET_INVALID -1

#if defined (_3DS)
    #define SO_KEEPALIVE 0x0008
    #define SO_DONTROUTE 0x0010
#endif

#define WAITFD_R POLLIN
#define WAITFD_W POLLOUT
#define WAITFD_C (POLLIN | POLLOUT)

#define UDP_DATASIZE 0x2000
#define BUF_DATASIZE 0x2000
