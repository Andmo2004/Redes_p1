// C++ libraries
#include <iostream>
#include <map>

// C libraries
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

// nuestras librerias
#include "bj_server.hpp"
#include "funciones.hpp"

#define MSG_SIZE 250
#define MAX_CLIENTS 30

using namespace std;

/*ELIMINAR USUARIOS DE VECTOR<USUARIO> CUANDO SE SALGAN DEL SERVER*/

int main (){
    
    vector<Carta> baraja1;
    vector<Mesa> mesas;
    mesas.resize(10);

    vector<Usuario> usuarios;
    usuarios.resize(MAX_CLIENTS);
    
    map<string, string> userData;
    rellenarVectorUsuario(userData);

    auto it = userData.begin();
    for (int i = 0; it != userData.end() && i < userData.size(); ++i, ++it)
    {
        usuarios[i].username += it->first;
        usuarios[i].password += it->second;

        /*  [DEBUG] BIEN  */ 
        // cout << "Usuario " << i + 1 << ": " << usuarios[i].username << "\n"
        //     << "Contraseña: " << usuarios[i].password << endl; 
    }
    mostrarUserData(userData);


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
    string temporal;
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
	signal(SIGINT, manejador);
    
	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1) {
        // Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
        auxfds = readfds;
        salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);

        if (salida == -1) {
            perror("Error en select");
            exit(EXIT_FAILURE);
        }

        if(salida > 0) {
            
            for(i = 0; i < FD_SETSIZE; i++) {
                // Buscamos el socket por el que se ha establecido la comunicación
                if(FD_ISSET(i, &auxfds)) {

                    if(i == sd) { // Nueva conexión
                        if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1) {
                            perror("-Err. Error aceptando peticiones");
                        } else {
                            if(numClientes < MAX_CLIENTS) {

                                arrayClientes[numClientes] = new_sd;
                                usuarios[numClientes].estado = CONECTADO;
                                usuarios[numClientes].id = new_sd;
                                numClientes++;

                                FD_SET(new_sd, &readfds);
                                
                                //send_to_user(new_sd, buffer, sizeof(buffer), "+Ok. Usuario conectado\n");
                                //memset(buffer, 0, sizeof(buffer));
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "+Ok. Usuario conectado\n");
                                send(new_sd,buffer,sizeof(buffer),0);

                                printf("Nuevo cliente conectado: %d\n", new_sd);

                            } else {
                                //send_to_user(new_sd, buffer, sizeof(buffer), "-Err. Demasiados clientes conectados\n");
                                memset(buffer, 0, sizeof(buffer));
                                strcpy(buffer, "-Err. Demasiados clientes conectados\n");
                                send(new_sd,buffer,sizeof(buffer),0);
                                
                                close(new_sd);
                            }
                        }
                    } else if (i == 0) { // Información de teclado
                        //bzero(buffer, sizeof(buffer))
                        memset(buffer, 0, sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);
                        
                        if(strcmp(buffer, "SALIR\n") == 0) {
                            for(j = 0; j < numClientes; j++) {

                                //send_to_user(arrayClientes[j], buffer, sizeof(buffer), "+Ok. Desconexión servidor\n");
                                memset(buffer, 0, sizeof(buffer));
                                strcpy(buffer, "+Ok. Desconexión servidor\n");
                                send(arrayClientes[j],buffer,sizeof(buffer),0);

                                close(arrayClientes[j]);
                                FD_CLR(arrayClientes[j], &readfds);
                            }
                            close(sd);
                            exit(-1);
                        }

                        // Implementar envío de mensajes a los clientes

                    } else { // Manejo de clientes conectados
                        //bzero(buffer, sizeof(buffer));
                        memset(buffer, 0, sizeof(buffer));

                        recibidos = recv(i, buffer, sizeof(buffer), 0);
                        
                        if(recibidos > 0) {

                            /* Registro */
                            if(buscar_palabra(buffer, "REGISTRO")) {
                                /* HACER UN BUCLE QUE BUSQUE EL USUARIOS[K].id == i */
                                if(usuarios[i].estado == CONECTADO){
                                    char username[50], password[50];
                                    if(sscanf(buffer, "REGISTRO -u %s -p %s", username, password) == 2) {
                                        if(updateUserData(string(username), string(password), userData)) {
                                            
                                            //send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario registrado correctamente, sal e inicia sesion\n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Usuario registrado correctamente, sal e inicia sesion\n");
                                            send(i,buffer,sizeof(buffer),0);


                                            userData.insert(make_pair(string(username), string(password)));
                                            mostrarUserData(userData);
                                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                                            
                                        } else {
                                            //send_to_user(i, buffer, sizeof(buffer), "-Err. El usuario ya esta registrado\n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "-Err. El usuario ya esta registrado\n");
                                            send(i,buffer,sizeof(buffer),0);

                                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                                        }
                                    } else {
                                        //send_to_user(i, buffer, sizeof(buffer), "-Err. La informacion no se ha introducido correctamente\n");
                                        memset(buffer, 0, sizeof(buffer));
                                        strcpy(buffer, "-Err. La informacion no se ha introducido correctamente\n");
                                        send(i,buffer,sizeof(buffer),0);

                                        salirCliente(i, &readfds, &numClientes, arrayClientes);
                                    }
                                } else {
                                    //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde al Registro\n");
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde al Registro\n");
                                    send(i,buffer,sizeof(buffer),0);
                                }
                            }

                            /* Inicio de sesión */
                            if(buscar_palabra(buffer, "USUARIO")) {
                                if(usuarios[i].estado == CONECTADO)
                                {
                                    char temp[50], username[50];
                                    if(sscanf(buffer, "%s %s", temp, username) == 2) {
                                        if(!existe_usuario(string(username), userData)) {

                                            //send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario no existente \n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "-Err. Nombre de usuario no existente \n");
                                            send(i,buffer,sizeof(buffer),0);


                                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                                        } else {
                                            // send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario correcto\n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Usuario correcto\n");
                                            send(i,buffer,sizeof(buffer),0);

                                            usuarios[i].estado = USUARIO_CORRECTO;
                                            printf("Usuario %s validado, estado: %d\n", username, usuarios[i].estado);
                                            temporal = string(username); 
                                        }
                                    }                                   
                                } else {
                                    //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde al Inicio de Sesion\n");
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde al Inicio de Sesion\n");
                                    send(i,buffer,sizeof(buffer),0);
                                }
                            }

                            if(buscar_palabra(buffer, "PASSWORD")) {
                                if(usuarios[i].estado==USUARIO_CORRECTO){
                                    char temp[50], password[50];

                                    if(sscanf(buffer, "%s %s", temp, password) == 2) {

                                        if(!contrasenia_correcta(temporal, string(password), userData)) {

                                            // send_to_user(i, buffer, sizeof(buffer), "-Err. Contrasenia incorrecta\n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "-Err. Contrasenia incorrecta\n");
                                            send(i,buffer,sizeof(buffer),0);


                                        } else {
                                            // send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario validado\n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Usuario validado\n");
                                            send(i,buffer,sizeof(buffer),0);

                                            usuarios[i].estado = USUARIO_VALIDADO;
                                            printf("Contraseña correcta, estado: %d\n", usuarios[i].estado);
                                        }
                                    }
                                } else {
                                    //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde a Correcto\n");
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde a Correcto\n");
                                    send(i,buffer,sizeof(buffer),0);
                                }
                            }

                            /* Iniciar partida */
                            if(buscar_palabra(buffer, "INICIAR-PARTIDA")) {
                                if(usuarios[i].estado == USUARIO_VALIDADO){
                                    usuarios[i].estado = INICIAR_PARTIDA;
                                    printf("Iniciando partida, estado: %d\n", usuarios[i].estado);
                                    
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Entrando en la cola para jugar...\n");
                                    send(i,buffer,sizeof(buffer),0);                      

                                    /**/

                                } else {
                                    //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde a Validacion\n");
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde a Validacion\n");
                                    send(i,buffer,sizeof(buffer),0);
                                }
                            }

                            /* Salir */
                            if(strcmp(buffer, "SALIR\n") == 0) {
                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                            }
                        }

                        // Si el cliente introdujo ctrl+c
                        if(recibidos == 0) {
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                            salirCliente(i, &readfds, &numClientes, arrayClientes);
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

// void send_to_user(int i, char *buffer, size_t buffer_size, const char *msg)
// {
//     bzero(buffer, buffer_size);  // Podemos usar memset(buffer, 0, buffer_size); en lugar de bzero
//     strncpy(buffer, msg, buffer_size - 1);  // buffer size-1 Asegurarse de no desbordar el buffer
//     send(i, buffer, strlen(buffer), 0); 
// };
