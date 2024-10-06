#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 9999    // The port to connect to
//#define SERVER_IP "137.184.10.4"  // The IP address to connect to (localhost for this example)
#define SERVER_IP "127.0.0.1"  // The IP address to connect to (localhost for this example)
#define MESSAGE "Hello from the client!"

int main(int argc, char *argv[]) {
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
    server_addr.sin_port = htons(SERVER_PORT);  // Set port, converting to network byte order

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
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
	char message[1024];
	strcpy(message,argv[1]);
    if (send(sock, message, strlen(message), 0) == -1) {
        perror("Send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Message sent to server: %s\n", message);

    // Close the socket
    close(sock);

    return 0;
}

