#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Unix standard functions (e.g., close)
#include <arpa/inet.h> // Networking functions for IPv4 (e.g., socket, bind, etc.)

#define PORT 9999

// Declare functions
int initialize_server(int port);
void server_loop(int server_fd);
void process_request(int socket, char *);

int main() {
    // Redirect stdout to a log file
//    freopen("output.log", "a", stdout);
//    freopen("error.log", "a", stderr);
    
    // Flush stdout and stderr immediately after writing
//    setbuf(stdout, NULL);
//    setbuf(stderr, NULL);

    // initialize the server and get the server file descriptor
    int server_fd = initialize_server(PORT);
    if (server_fd < 0) {
        fprintf(stderr, "Server initialization failed.\n");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
	printf("initialized receiver\n");
#endif

    // Enter the server loop to listen for connections and process requests
    server_loop(server_fd);

    // Close the server socket when done (although this will never be reached in the infinite loop)
    close(server_fd);
    return 0;
}

// Function to initialize the server
int initialize_server(int port) {
    int server_fd;
    struct sockaddr_in address;

    // Create a socket: AF_INET = IPv4, SOCK_STREAM = TCP, 0 = Default protocol
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }

    // Set SO_REUSEADDR to reuse the port
	int opt=1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any IP address
    address.sin_port = htons(port); // Convert the port number to network byte order

    // Bind the socket to the specified IP address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }

    // Put the server socket in a passive mode to listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return -1;
    }

    return server_fd;
}

void server_loop(int server_fd) {
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

#ifdef DEBUG
		printf("starting server_loop\n");
#endif

    // Main loop to handle incoming requests, runs infinite
	char response_buffer[1024];
	strcpy(response_buffer,"");
    while (strcmp(response_buffer,"done")!=0) {
        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }
#ifdef DEBUG
		printf("received data\n");
#endif

        // Process the client's request
        process_request(new_socket,response_buffer);

		printf("got : %s\n",response_buffer);

        // Close the server socket when done (although this will never be reached in the infinite loop)
        close(new_socket);
    }
}

// Function to process the incoming HTTP request and respond
void process_request(int socket,char *response_s) {
    char buffer[1024] = {0}; // Buffer to hold incoming data

    // Response for handling OPTIONS HTTP requests (used for CORS preflight requests)
    char *options_response = "HTTP/1.1 204 No Content\r\n"
                             "Access-Control-Allow-Origin: *\r\n"
                             "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                             "Access-Control-Allow-Headers: Content-Type\r\n"
                             "\r\n";
    
    // Standard response for a successful request
    char *response = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/plain\r\n"
                     "Access-Control-Allow-Origin: *\r\n"
                     "\r\n"
                     "Request received.";

    // Read data from the client
    ssize_t valread = read(socket, buffer, sizeof(buffer) - 1);

	printf("read %d characters : %s\n",valread,buffer);
	strcpy(response_s,buffer);

#if 0
    // Check if the request is an OPTIONS request (CORS preflight)
    if (strstr(buffer, "OPTIONS") != NULL) {
        // Respond with the pre-defined OPTIONS response
        write(socket, options_response, strlen(options_response));
        return;
    }

    // Find the beginning of the request body (after the HTTP headers)
    char *body = strstr(buffer, "\r\n\r\n"); 
    if (body) {
        body += 4; // Move the pointer past the "\r\n\r\n" to the start of the body

        // Try to find a JSON key called "key" in the body
        char *key_start = strstr(body, "\"key\":\"");
        if (key_start) {
            key_start += 7; // Move the pointer past the '"key":"' to the actual value
            char *key_end = strchr(key_start, '"'); // Find the closing quote of the key value
            if (key_end) {
                *key_end = '\0'; // Null-terminate the key value
                printf("%s\n", key_start); // Log the key value to the log file
            } else {
                // If no closing quote was found, log an error message
                fprintf(stderr, "End quote not found.\n");;
            }
        } else {
            // If no "key" is found in the body, log an error message
            fprintf(stderr, "Key not found.\n");
        }
    } else {
        // If no body is found in the request, log an error message
        fprintf(stderr, "No body found.\n");
    }

    // Send the standard response back to the client
    write(socket, response, strlen(response));
#endif	
}
