#ifndef BJ_SERVER_H
#define BJ_SERVER_H

void send_to_user(int i, char *buffer, size_t buffer_size, const char *msg);
void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);

#endif