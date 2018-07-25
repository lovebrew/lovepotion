#pragma once

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>

#define SOCKET_BUFFERSIZE 0x2000
#define SO_BROADCAST	  0x0020
#define SO_DONTROUTE	  0x0010