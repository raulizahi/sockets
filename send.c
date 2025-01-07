/*
MIT License

Copyright (c) 2024 Seagull Concepts, LLC.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 9999    // The port to connect to
//#define SERVER_IP "137.184.10.4"  // The IP address to connect to (localhost for this example)
//#define SERVER_IP "127.0.0.1"  // The IP address to connect to (localhost for this example)

#define ARG_COUNT 	3 // IP_ADDRESS and PORT_NUMBER
#define PROGRAM		0
#define IP_ADDRESS	1
#define PORT_NUMBER	2
#define MESSAGE		3

#define ERROR_ARGS	-1

int main(int argc, char *argv[]) {
	// check command line parameters
	if (argc!=(ARG_COUNT+1))
	{
		fprintf(stderr,"usage : %s IPV4_address port_number\n",argv[PROGRAM]);
		return ERROR_ARGS;
	}
	// get IP address and port number
	char ip_address_s[256];
	strcpy(ip_address_s,argv[IP_ADDRESS]);
	uint16_t	port_number_ui16;
	port_number_ui16 = (uint16_t)atoi(argv[PORT_NUMBER]);

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
	printf("sock=%d\n",sock);
#endif

    // Define server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));	// reset server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number_ui16);  // Set port, converting to network byte order

    if (inet_pton(AF_INET, ip_address_s, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
	printf("server address set\n");
#endif

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
	printf("connected to socket\n");
#endif

    // Send data to the server
	char message_s[1024];
	strcpy(message_s,argv[MESSAGE]);
    if (send(sock, message_s, strlen(message_s), 0) == -1) {
        perror("Send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Message sent to server: %s\n", message_s);

	// read from socket
	char buffer[1024] = {0}; // Buffer to hold incoming data
	ssize_t valread = read(sock, buffer, sizeof(buffer) - 1);
	printf("read %zd characters from server : %s\n",valread,buffer);

    // Close the socket
    close(sock);

    return 0;
}

