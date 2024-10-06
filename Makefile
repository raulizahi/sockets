all : server send
debug: server_debug send_debug
clean:
	rm ./server ./send
server : server.c
	gcc -O2 -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o server server.c
server_debug : server.c
	gcc -DDEBUG -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o server_debug server.c
send : send.c
	gcc -O2 -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o send send.c
send_debug : send.c
	gcc -DDEBUG -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o send_debug send.c
