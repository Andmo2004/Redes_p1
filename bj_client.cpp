#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

/* El cliente manda cadena de textos al servidor hasta que manda la cadena FIN */ 

int main ( )
{
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int descrip;
	struct sockaddr_in sockname;
	char buffer[100];
	socklen_t len_sockname;
	int salir = 0;
	
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	descrip = socket (AF_INET, SOCK_STREAM, 0);
	if (descrip == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2060);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(descrip, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}

	printf("Conectado al servidor.\n");

	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do
	{
		salir = 0;
		puts("Teclee el mensaje a transmitir");
		fgets(buffer, sizeof(buffer),stdin);
		buffer[strlen(buffer)-1] = '\0';
			
		if(strcmp(buffer, "FIN") == 0)
			salir = 1;

		// Enviar un mensaje al servidor
		if (send(descrip, buffer, sizeof(buffer), 0) == -1) {
        		perror("Error al enviar el mensaje");
       			 exit(1);
   		 }

		// Recibir respuesta del servidor
   		bzero(buffer, sizeof(buffer));
   		if (recv(descrip, buffer, sizeof(buffer), 0) == -1) {
        		perror("Error al recibir la respuesta");
        		exit(1);
    		}

    		printf("Respuesta del servidor: %s\n", buffer);

				
	}while(!salir);
	
	// Cerrar el socket del cliente	
	close(descrip);
	return 0;
}
	