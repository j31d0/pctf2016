all: client.c server.c server_select.c
	gcc -o client client.c
	gcc -o server server.c
	gcc -o server_select server_select.c

debug: client.c server.c server_select.c
	gcc -D DEBUG -o client_debug client.c
	gcc -D DEBUG -o server_debug server.c
	gcc -D DEBUG -o server_select_debug server_select.c
