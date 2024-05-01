#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <netdb.h>

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

void* handle_client(void* p_client) {
	int client = *((int*)p_client);
	free(p_client);

	char buffer[1024] = {0};
	long bytes = 0;
	int msg = 0;
	while (1) {
		int size = read(client, buffer, 1023);
		if (size == 0)
			break;
		if (strncmp(buffer, (char *)"\\q", 2) == 0)
			break;
		else if (strncmp(buffer, (char *)"\\s", 2) == 0) {
			size = sprintf(buffer, "Bytes: %ld\nMsg: %d\n", bytes, msg);
		}
		send(client, buffer, size, 0);
		bytes += size;
		msg += 1;
	}
	close(client);
	return NULL;
}

int main() {
	struct sockaddr_in my_addr, peer_addr;
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket == -1)
		handle_error("socket");

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(6000);
	
	if (bind(tcp_socket, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
		handle_error("bind");
	
	if (listen(tcp_socket, 50) == -1)
		handle_error("listen");

	int peer_addrlen = sizeof(peer_addr);
	int tcp_connection;
	
	while (1) {
		printf("Waiting for connection\n");
		tcp_connection = accept(tcp_socket, (struct sockaddr*) &peer_addr, &peer_addrlen);
		if (tcp_connection == -1)
			continue;

		char host[NI_MAXHOST], service[NI_MAXSERV];
		int s = getnameinfo((struct sockaddr *) &peer_addr, peer_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
		if (s == 0)
			printf("Connected with %s:%s\n", host, service);

		pthread_t t;
		int* p_client = malloc(sizeof(int));
		*p_client = tcp_connection;
		pthread_create(&t, NULL, handle_client, p_client);
	}

	if (close(tcp_socket) == -1)
		handle_error("close");

	return 0;
}