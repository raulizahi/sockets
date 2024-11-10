// Socket Server
// By: Raul Izahi Lopez, Seagull Concepts, LLC.
// Creation: Oct 6, 2024
// Documentation from: UNIX Network Programming, by W. Richard Stevens and iOS's man

/*		Function dialog:
		TCP Client			TCP Server
							socket()
							bind()
							listen()
							accept()
		socket()
		connect()
		write()				read()
							write()
		read()
		close()				read()
							close()
*/

#include <stdlib.h>		// C Standard library
#include <string.h>		// String management
#include "init_server.h" // necessary includes for server

// command line
#define	ARGS_COUNT	1

// argv array indexes
#define PROGRAM	0 // executable name
#define PORT	1 // port number

// error return values
# define ARGC_ERROR	-1

// function prototypes
int 	init_server(in_port_t port_t); // returns integer socket descriptor
void 	server_loop(int socket_i);
void 	process_request(int socket, char *);

int main(int argc, char *argv[]) {	// accept command line arguments
    // Redirect stdout to a log file
//    freopen("output.log", "a", stdout);
//    freopen("error.log", "a", stderr);
    
    // Flush stdout and stderr immediately after writing
//    setbuf(stdout, NULL);
//    setbuf(stderr, NULL);

	// check command line argument count
	if (argc!=(ARGS_COUNT+1))
	{
		fprintf(stderr,"Usage : %s port_number\n",argv[PROGRAM]);
		return ARGC_ERROR;
	} // check argc

	// extract port to use
	in_port_t	port_t;
	port_t = (in_port_t)atoi(argv[PORT]);

    // initialize the server and get the server socket descriptor
    int socket_i = init_server(port_t);
#ifdef DEBUG
	printf("got socket_i = %d\n",socket_i);
#endif
    if (socket_i < 0) {
        fprintf(stderr, "Server initialization failed.\n");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
	printf("initd receiver\n");
#endif

    // Enter the server loop to listen for connections and process requests
    server_loop(socket_i);

    // Close the server socket when done (although this will never be reached in the infinite loop)
    close(socket_i);
    return 0;
}

// Function to init the server
#if 0
int init_server(in_port_t port_t) {
    int socket_i;				// socket descriptor : int socket(int domain, int type, int protocol);
	int socket_domain_i;
	int socket_type_i;
	int socket_protocol_i;
    struct sockaddr_in address_st;

	socket_domain_i = AF_INET;
	socket_type_i 	= SOCK_STREAM; 	// sequenced, reliable, two-way connection based byte streams. 
									// full-duplex byte streams, similar to pipes.

    // Create a socket: AF_INET = IPv4, SOCK_STREAM = TCP, 0 = Default protocol
    if ((socket_i = socket(socket_domain_i, socket_type_i, 0)) == 0) {
        fprintf(stderr,"socket failed\n");
        return -1;
    }

    // Set SO_REUSEADDR to reuse the port
	int opt=1;
    if (setsockopt(socket_i, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        fprintf(stderr,"setsockopt failed\n");
        close(socket_i);
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address_st.sin_family = AF_INET; // IPv4
    address_st.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any IP address
    address_st.sin_port = htons(port_t); // Convert the port number to network byte order

    // Bind the socket to the specified IP address and port
    if (bind(socket_i, (struct sockaddr *)&address_st, sizeof(address_st)) < 0) {
        fprintf(stderr,"bind failed\n");
        return -1;
    }

    // Put the server socket in a passive mode to listen for incoming connections
    if (listen(socket_i, 3) < 0) {
        fprintf(stderr,"listen\n");
        return -1;
    }

    return socket_i;
}
#endif

void server_loop(int socket_i) {
    int connfd_i;	// connected descriptor: unique per client connected to server. UNIX Network Programming, by Stevens, page 14
    struct sockaddr_in address_st;
    int addrlen = sizeof(address_st);

#ifdef DEBUG
		printf("starting server_loop\n");
#endif

    // Main loop to handle incoming requests, runs infinite
	char response_buffer[1024];
	strcpy(response_buffer,"");
#ifdef DEBUG
	printf("accept passed\n");
#endif
    while (strcmp(response_buffer,"done")!=0) {
		// Accept a new connection
		if ((connfd_i = accept(socket_i, (struct sockaddr *)&address_st, (socklen_t*)&addrlen)) < 0) {
			fprintf(stderr,"accept failed\n");
			return;
		}
#ifdef DEBUG
		printf("received data\n");
#endif
        // Process the client's request
        process_request(connfd_i,response_buffer);
#ifdef DEBUG
		printf("got : >%s<\n",response_buffer);
#endif

        // Close the server socket when done (although this will never be reached in the infinite loop)
        close(connfd_i);
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

	printf("read %zd characters : %s\n",valread,buffer);
	strcpy(response_s,buffer);
	// printf, respond with something
	char message_s[1024];
	strcpy(message_s,"this is the server's response");

   if (send(socket, message_s, strlen(message_s), 0) == -1) {
		perror("Send failed");
		close(socket);
		exit(EXIT_FAILURE);
    }

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
