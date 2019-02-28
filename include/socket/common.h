#pragma once

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>

#include <unistd.h>

#define SOCKET_BUFFERSIZE 0x2000

#define TCP_NODELAY 1

#ifndef SO_KEEPALIVE
    #define SO_KEEPALIVE 0x0008
#endif

#ifndef SO_DONTROUTE
    #define SO_DONTROUTE 0x0010
#endif