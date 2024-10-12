#ifndef BJ_SERVER_H
#define BJ_SERVER_H

void send_to_user(int i, char *buffer, size_t buffer_size, const char *msg);
bool buscar_palabra(const char *cadena, const char *palabra);
bool existe_usuario(const char* usuario, const char** arrayClientes, size_t numClientes);

#endif