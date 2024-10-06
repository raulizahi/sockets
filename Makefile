all : rec send
debug: rec_debug send_debug
clean:
	rm ./rec ./send
rec : rec.c
	gcc -O2 -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o rec rec.c
rec_debug : rec.c
	gcc -DDEBUG -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o rec rec.c
send : send.c
	gcc -O2 -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o send send.c
send_debug : send.c
	gcc -DDEBUG -Wall -Wextra -std=c99 -Wold-style-definition -Wstrict-prototypes -Wmissing-prototypes -pedantic -o send send.c
