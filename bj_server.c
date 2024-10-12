#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "bj_server.h"

#define MSG_SIZE 250
#define MAX_CLIENTS 30


/***** hola miguel angel ****** */

/*
 * El servidor ofrece el servicio de un chat rico
 me gusta el sexo cabron
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);


int main (){
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
	fd_set readfds, auxfds;
   	int salida;
   	int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;

   	//contadores
    int i,j,k;
	int recibidos;
   	char identificador[MSG_SIZE];
    int on, ret;

	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1){
		
		perror("-Err.No se puede abrir el socket cliente\n");
    	exit (1);	
	}
    
    	// Activaremos una propiedad del socket para permitir· que otros
    	// sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    	// Esto permite· en protocolos como el TCP, poder ejecutar un
    	// mismo programa varias veces seguidas y enlazarlo siempre al
   	 // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    	// quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));


	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2060);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("-Err.Error en la operación bind");
		exit(1);
	}
	
   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,1) == -1){
		perror("-Err.Error en la operación de listen");
		exit(1);
	}

	
    printf("El servidor está esperando conexiones...\n");	//Inicializar los conjuntos fd_set
    	
	FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
	FD_SET(sd,&readfds);
	FD_SET(0,&readfds);
    
   	
    //Capturamos la señal SIGINT (Ctrl+c)
	signal(SIGINT,manejador);
    
	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1){
            
            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
            
            auxfds = readfds;
            
            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
            
            if(salida > 0){
                
                
                for(i=0; i<FD_SETSIZE; i++){
                    
                    //Buscamos el socket por el que se ha establecido la comunicación
                    if(FD_ISSET(i, &auxfds)) {
                        
                        if( i == sd){
                            
                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("-Err.Error aceptando peticiones");
                            }

                            else{

                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes] = new_sd;
                                    numClientes++;
                                    FD_SET(new_sd,&readfds);

                                    send_to_user(new_sd, buffer, sizeof(buffer), "+Ok. Usuario conectado\n");
									// bzero(buffer,sizeof(buffer));
                                    // strcpy(buffer, "+Ok. Usuario conectado\n");
                                    // send(new_sd,buffer,sizeof(buffer),0);
                                
                                    for(j=0; j<(numClientes-1);j++){
                                    
                                        sprintf(buffer, "+Ok. Nuevo Cliente conectado en <%d>",new_sd);
                                        send_to_user(arrayClientes[j], buffer, sizeof(buffer), buffer);
                                        // bzero(buffer,sizeof(buffer));
                                        // send(arrayClientes[j],buffer,sizeof(buffer),0);
                                    }
                                }
                                else
                                {
                                    send_to_user(new_sd, buffer, sizeof(buffer), "-Err. Demasiados clientes conectados\n");
                                    // bzero(buffer,sizeof(buffer));
                                    // strcpy(buffer,"-Err. Demasiados clientes conectados\n");
                                    // send(new_sd,buffer,sizeof(buffer),0);
                                    close(new_sd);
                                } 
                            }
                        }
                        else if (i == 0){
                            //Se ha introducido información de teclado
                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);
                            
                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){
                             
                                for (j = 0; j < numClientes; j++){

                                    send_to_user(arrayClientes[j], buffer, sizeof(buffer), "+Ok. Desconexión servidor\n");
                                    // bzero(buffer, sizeof(buffer));
                                    // strcpy(buffer,"+Ok. Desconexión servidor\n"); 
                                    // send(arrayClientes[j],buffer , sizeof(buffer),0);
                                    close(arrayClientes[j]);
                                    FD_CLR(arrayClientes[j],&readfds);
                                }
                                close(sd);
                                exit(-1);     
                            }
                            //Mensajes que se quieran mandar a los clientes (implementar)   
                        } 
                        else{
                            int num=0,num2=0;
                            bzero(buffer,sizeof(buffer));
                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                            if(recibidos > 0){

                                if(sscanf(buffer,"USUARIO %d",&num)!=1){

                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Usuario incorrecto\n");
                                    // bzero(buffer,sizeof(buffer));
                                    // strcpy(buffer, "-Err. Usuario incorrecto\n");
                                    // send(i,buffer,sizeof(buffer),0);
                                }
                                else{
                                    
                                    if(num==i){

                                        send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario correcto\n");
                                        // bzero(buffer,sizeof(buffer));
                                        // strcpy(buffer, "+Ok. Usuario correcto\n");
                                        // send(i,buffer,sizeof(buffer),0);

                                        bzero(buffer,sizeof(buffer));
                                        recibidos = recv(i,buffer,sizeof(buffer),0);

                                        if(recibidos>0){

                                            if(sscanf(buffer,"PASSWORD %d",&num2)!=1){

                                                send_to_user(i, buffer, sizeof(buffer), "-Err. Error en la validación\n");
                                                // bzero(buffer,sizeof(buffer));
                                                // strcpy(buffer, "-Err. Error en la validación\n");
                                                // send(i,buffer,sizeof(buffer),0);
                                            }
                                            else{

                                                if(num2==i){
                                                    
                                                    send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario validado\n");
                                                    // bzero(buffer,sizeof(buffer));
                                                    // strcpy(buffer, "+Ok. Usuario validado\n");
                                                    // send(i,buffer,sizeof(buffer),0);

                                                    bzero(buffer,sizeof(buffer));
                                                    recibidos = recv(i,buffer,sizeof(buffer),0);
                                                    //seguir con el flujo
                                                    if(recibidos>0){

                                                    }
                                                }
                                                else{
                                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Error en la validación\n");
                                                    // bzero(buffer,sizeof(buffer));
                                                    // strcpy(buffer, "-Err. Error en la validación\n");
                                                    // send(i,buffer,sizeof(buffer),0);
                                                }
                                            }
                                        }
                                    }
                                    else{
                                        
                                        send_to_user(i, buffer, sizeof(buffer), "-Err. Usuario incorrecto\n");
                                        // bzero(buffer,sizeof(buffer));
                                        // strcpy(buffer, "-Err. Usuario incorrecto\n");
                                        // send(i,buffer,sizeof(buffer),0);
                                    }

                                }

                                if(sscanf(buffer,"REGISTRO -u %d -p %d",&num,&num2)!=2){

                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Error en registro\n");
                                    // bzero(buffer,sizeof(buffer));
                                    // strcpy(buffer, "-Err. Error en registro\n");
                                    // send(i,buffer,sizeof(buffer),0);
                                }
                                else{
                                    //comprobación si el user existe ya?

                                }
                                
                                //final
                                if(strcmp(buffer,"SALIR\n") == 0){
                                    
                                    salirCliente(i,&readfds,&numClientes,arrayClientes);
                                }
                                else{
                                    
                                    // %.240s: Esto asegura que, como máximo, se copiarán 240 caracteres de 
                                    // buffer (dejando espacio para los demás elementos del formato 
                                    // como <%d>: y el terminador nulo \0).
                                    snprintf(identificador, sizeof(identificador), "<%d>: %.240s", i, buffer);
                                    // sprintf(identificador,"<%d>: %s",i,buffer);
                                    
                                    bzero(buffer,sizeof(buffer));

                                    strcpy(buffer,identificador);

                                    printf("%s\n", buffer);

                                    for(j=0; j<numClientes; j++){
                                        if(arrayClientes[j] != i){
                                            send(arrayClientes[j],buffer,sizeof(buffer),0);
                                        }
                                    }
                                }
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos== 0){
                                
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                        }
                    }
                }
            }
		}

	close(sd);
	return 0;
	
}

void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]){
  
    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente <%d>",socket);
    
    for(j=0; j<(*numClientes); j++)
        if(arrayClientes[j] != socket)
            send(arrayClientes[j],buffer,sizeof(buffer),0);
}


void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
};

void send_to_user(int i, char *buffer, size_t buffer_size, const char *msg)
{
    bzero(buffer, buffer_size);  // Podemos usar memset(buffer, 0, buffer_size); en lugar de bzero
    strncpy(buffer, msg, buffer_size - 1);  // buffer size-1 Asegurarse de no desbordar el buffer
    send(i, buffer, strlen(buffer), 0); 
};

bool buscar_palabra(const char *cadena, const char *palabra) 
{
    if (strstr(cadena, palabra) != NULL) {return true;}
    return false;
};

bool existe_usuario(const char* usuario, const char** arrayClientes, size_t numClientes)
{
    for(size_t i=0; i < numClientes; ++i)
    {
        if(strcmp(usuario, arrayClientes[i])){
            return true;
        }
    }
    return false;
};