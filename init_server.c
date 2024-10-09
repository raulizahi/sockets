#include "init_server.h"
// function prototype
int init_server(in_port_t port_t);
void get_socket_data_s(int,char *);
// Function to init the server
int init_server(in_port_t port_t) 
{
    int socket_i;               // socket descriptor : int socket(int domain, int type, int protocol);
    int socket_domain_i;
    int socket_type_i;
    int socket_protocol_i;
    struct sockaddr_in address_st;

    socket_domain_i = AF_INET;		// IPV4, IPV6 is AF_INET6
    socket_type_i   = SOCK_STREAM;  // sequenced, reliable, two-way connection based byte streams. 
                                    // full-duplex byte streams, similar to pipes.
	socket_protocol_i = 0;			// Default protocol except for raw_socket

    // Create a socket: AF_INET = IPv4, SOCK_STREAM = TCP, 0 = Default protocol
    if ((socket_i = socket(socket_domain_i, socket_type_i,socket_protocol_i )) == 0) 
	{
        fprintf(stderr,"socket failed\n");
        return SERVER_ERROR_SOCKET; 
    }   

    // Set SO_REUSEADDR to reuse the port
    int opt=1;
    if (setsockopt(socket_i, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
	{
        fprintf(stderr,"setsockopt failed\n");
        close(socket_i);
        return SERVER_ERROR_OPT;
    }   

    // Define the server address
    address_st.sin_family = AF_INET; // IPv4
    address_st.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any IP address
    address_st.sin_port = htons(port_t); // Convert the port number to network byte order

    // Bind the socket to the specified IP address and port
    if (bind(socket_i, (struct sockaddr *)&address_st, sizeof(address_st)) < 0) 
	{
        fprintf(stderr,"bind failed\n");
        return SERVER_ERROR_BIND;
    }

    // Put the server socket in a passive mode to listen for incoming connections
    if (listen(socket_i, 3) < 0) 
	{
        fprintf(stderr,"listen failed\n");
        return SERVER_ERROR_LISTEN;
    }

    return socket_i;
}
void get_socket_data_s(int socket_i, char *data_s)
{
	char buffer_s[1024] = {0};	// data to be returned
	int new_socket;
    struct sockaddr_in address_st;
    int addrlen = sizeof(address_st);

	// accept
	if ((new_socket = accept(socket_i, (struct sockaddr *)&address_st, (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr,"accept failed\n");
	}
	// read
	ssize_t valread = read(new_socket, buffer_s, sizeof(buffer_s) - 1);
#ifdef DEBUG
	printf("read %zd characters : %s\n",valread,buffer_s);
#endif
	strcpy(data_s,buffer_s);
}
