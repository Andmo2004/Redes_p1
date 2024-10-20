#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>


int main()
{
    int sd;
    struct sockaddr_in sockname;
    char buffer[250];
    socklen_t len_sockname;
        fd_set readfds, auxfds;
        int salida;
        int fin = 0;

    // Se abre el socket 
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }
    // Configuración del servidor
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2060);
    sockname.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Se solicita la conexión con el servidor
    len_sockname = sizeof(sockname);

    if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1) {
        perror("Error de conexión");
        exit(1);
    }

    // Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0, &readfds);  // Entrada estándar (teclado)
    FD_SET(sd, &readfds); // Socket del servidor

    // Bucle principal
    do {
        auxfds = readfds;
        salida = select(sd + 1, &auxfds, NULL, NULL, NULL);

        if (salida == -1) {
            perror("Error en select");
            exit(EXIT_FAILURE);
        }
        
        // Mensaje desde el servidor
        if (FD_ISSET(sd, &auxfds)) {
            bzero(buffer, sizeof(buffer));
            int bytes_recibidos = recv(sd, buffer, sizeof(buffer), 0);
            
            if (bytes_recibidos == -1) {
                perror("Error al recibir datos del servidor");
                exit(1);
            }
            
            if (bytes_recibidos == 0) {
                printf("Conexión cerrada por el servidor\n");
                fin = 1;  // El servidor cerró la conexión
                break;
            }

            // Imprimir el mensaje recibido tal como es, sin depuración
            printf("\n%s\n", buffer);

            // Manejar comandos desde el servidor
            if (strncmp(buffer, "Demasiados clientes conectados", 30) == 0) {
                fin = 1;
            }
            else if (strncmp(buffer, "Desconexión servidor", 20) == 0) {
                fin = 1;
            }
        } 
        else 
        {
            // Entrada por teclado
            if (FD_ISSET(0, &auxfds)) {
                bzero(buffer, sizeof(buffer));
                
                if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                    perror("Error al leer la entrada");
                }
                
                // Elimina salto de línea al final del input
                //buffer[strcspn(buffer, "\n")] = 0;

                // Verifica si el cliente quiere salir
                if (strcmp(buffer, "SALIR\n") == 0) {
                    fin = 1;
                }

                // Enviar el comando al servidor
                send(sd, buffer, sizeof(buffer), 0);
            }
        }
    } while (fin == 0);

    close(sd);

    return 0;
}