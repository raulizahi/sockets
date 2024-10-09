#include <stdio.h>
#include <unistd.h>     // Unix standard functions
#include <arpa/inet.h>  // Networking functions
#include <string.h>

#define SERVER_ERROR_SOCKET	-1
#define SERVER_ERROR_OPT	-2
#define SERVER_ERROR_BIND	-3
#define SERVER_ERROR_LISTEN	-4
