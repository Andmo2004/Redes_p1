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
    


    vector<Mesa> partidas;
    partidas.resize(10);
    inicializarEstadoPartidas(partidas);
    mostrarDatosPartidas(partidas);
    
    vector<Usuario> usuarios;
    usuarios.resize(MAX_CLIENTS);
    
    map<string, string> userData;
    rellenarVectorUsuario(userData);

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
    int i,j;
	int recibidos;
   	//char identificador[MSG_SIZE];
    int on;

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

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    perror("setsockopt failed");
    // Manejo del error si es necesario
    }



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

                        if (fgets(buffer, sizeof(buffer), stdin) == NULL){
                            perror("Error al leer la entrada");
                        }
                        
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
                        memset(buffer, 0,sizeof(buffer));
                        recibidos = recv(i, buffer, sizeof(buffer), 0);
                        
                        if(recibidos > 0) {

                            /* Registro */
                            if(buscar_palabra(buffer, "REGISTRO")) {
                                /* HACER UN BUCLE QUE BUSQUE EL USUARIOS[K].id == i */
                                int k = numUsuario(usuarios, i);
                                usuarios[k].id = i;
                                if(usuarios[k].estado == CONECTADO){
                                    char username[50], password[50];
                                    if(sscanf(buffer, "REGISTRO -u %s -p %s", username, password) == 2) {
                                        if(updateUserData(string(username), string(password), userData)) {
                                            
                                            //send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario registrado correctamente, sal e inicia sesion\n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Usuario registrado correctamente, sal e inicia sesion\n");
                                            send(usuarios[k].id,buffer,sizeof(buffer),0);


                                            //userData.insert(make_pair(string(username), string(password)));
                                            mostrarUserData(userData);
                                            salirCliente(usuarios[k].id, &readfds, &numClientes, arrayClientes);
                                            //sacarUsuarioDesconectado(usuarios, i);
                                            
                                        } else {
                                            //send_to_user(i, buffer, sizeof(buffer), "-Err. El usuario ya esta registrado\n");
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "-Err. El usuario ya esta registrado\n");
                                            send(usuarios[k].id,buffer,sizeof(buffer),0);

                                            salirCliente(usuarios[k].id, &readfds, &numClientes, arrayClientes);
                                            //sacarUsuarioDesconectado(usuarios, i);
                                        }
                                    } else {
                                        //send_to_user(i, buffer, sizeof(buffer), "-Err. La informacion no se ha introducido correctamente\n");
                                        memset(buffer, 0, sizeof(buffer));
                                        strcpy(buffer, "-Err. La informacion no se ha introducido correctamente\n");
                                        send(usuarios[k].id,buffer,sizeof(buffer),0);

                                        salirCliente(usuarios[k].id, &readfds, &numClientes, arrayClientes);
                                        //sacarUsuarioDesconectado(usuarios, i);
                                    }
                                } else {
                                    //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde al Registro\n");
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde al Registro\n");
                                    send(usuarios[k].id,buffer,sizeof(buffer),0);
                                }
                            }

                            /* Inicio de sesión */
                            if(buscar_palabra(buffer, "USUARIO")) {
                                int k = numUsuario(usuarios, i);
                                char temp[50], username[50];
                                int aux = (sscanf(buffer, "%s %s", temp, username));
                                if(usuarioIsConectado(usuarios, string(username))){

                                    cout << "-Err. El usuario " << string(username) << " ya esta conectado." << endl;
                                    salirCliente(i, &readfds, &numClientes, arrayClientes);
                                    //sacarUsuarioDesconectado(usuarios, i);

                                } else {
                                    cout << "Num clientes: " << i << " Letra k: " << k << endl;
                                    if(usuarios[k].estado == CONECTADO)
                                    {
                                        if(aux == 2) {
                                            if(!existe_usuario(string(username), userData)) {

                                                //send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario no existente \n");
                                                memset(buffer, 0, sizeof(buffer));
                                                strcpy(buffer, "-Err. Nombre de usuario no existente \n");
                                                send(i,buffer,sizeof(buffer),0);

                                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                                                //sacarUsuarioDesconectado(usuarios, i);

                                            } else {
                                                // send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario correcto\n");
                                                memset(buffer, 0, sizeof(buffer));
                                                strcpy(buffer, "+Ok. Usuario correcto\n");
                                                send(i,buffer,sizeof(buffer),0);

                                                usuarios[k].estado = USUARIO_CORRECTO;
                                                usuarios[k].username = string(username);


                                                printf("Usuario %s validado, estado: %d\n", username, usuarios[k].estado);
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
                            }

                            if(buscar_palabra(buffer, "PASSWORD")) {
                                int k = numUsuario(usuarios, i);
                                if(usuarios[k].estado==USUARIO_CORRECTO){
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

                                            usuarios[k].estado = USUARIO_VALIDADO;
                                            usuarios[k].password = string(password); // tampoco afecta mucho la vrd

                                            printf("Contraseña correcta, estado: %d\n", usuarios[k].estado);
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
                            if(strcmp(buffer, "INICIAR-PARTIDA\n") == 0){
                                int k = numUsuario(usuarios, i);
                                if(usuarios[k].estado == USUARIO_VALIDADO){
                                    usuarios[k].estado = INICIAR_PARTIDA;
                                    printf("Iniciando partida, estado: %d\n", usuarios[k].estado);

                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Entrando en la cola para jugar...\n");
                                    send(i, buffer, strlen(buffer), 0);  // Cambiado a strlen(buffer)

                                    printf("[DEBUG] Funcion buscarPartidaLibre\n");
                                    int numPartida = buscarPartidaLibre(partidas);
                                    
                                    printf("[DEBUG] numero de la partida: %d, del Jugador %s\n", numPartida, usuarios[k].username.c_str());

                                    if(numPartida != -1 && partidas[numPartida].estadoPartida == VACIA){  // Validación de numPartida
                                        partidas[numPartida].jugador1 = k;
                                        partidas[numPartida].estadoPartida = INCOMPLETA;

                                        usuarios[k].estado = WAITING;
                                        
                                        printf("[DEBUG] Usuario: %s\n   > Socket: %d\n   > Estado: %d\n   > NPartida:%d\n   > EstatusPartida: %d\n",
                                            usuarios[k].username.c_str(), usuarios[k].id, usuarios[k].estado, numPartida, partidas[numPartida].estadoPartida);

                                        memset(buffer, 0, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Esperando otro jugador\n");
                                        send(usuarios[k].id, buffer, strlen(buffer), 0);  // Cambiado a usuarios[k].id y strlen(buffer)
                                        break;
                                    } 
                                    if(partidas[numPartida].estadoPartida == INCOMPLETA){

                                        memset(buffer, 0, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Empieza la partida.\n");
                                        send(usuarios[k].id, buffer, strlen(buffer), 0);  // Cambiado a usuarios[k].id y strlen(buffer)

                                        partidas[numPartida].jugador2 = k;
                                        partidas[numPartida].estadoPartida = EMPEZADA;

                                        usuarios[partidas[numPartida].jugador1].estado = JUGANDO;
                                        usuarios[partidas[numPartida].jugador2].estado = JUGANDO;

                                        vector<Carta> barajaPartida;
                                        rellenarBaraja(barajaPartida);

                                        partidas[numPartida].baraja = barajaPartida;

                                        // DAR DOS CARTAS AL JUGADOR 1
                                        partidas[numPartida].cartasJugador1.push_back(partidas[numPartida].baraja.back());
                                        partidas[numPartida].baraja.pop_back();

                                        partidas[numPartida].cartasJugador1.push_back(partidas[numPartida].baraja.back());
                                        partidas[numPartida].baraja.pop_back();

                                        partidas[numPartida].manoJugador1 = calcularValorMano(partidas[numPartida].cartasJugador1);
                                        
                                        // DAR DOS CARTAS AL JUGADOR 2
                                        partidas[numPartida].cartasJugador2.push_back(partidas[numPartida].baraja.back());
                                        partidas[numPartida].baraja.pop_back();

                                        partidas[numPartida].cartasJugador2.push_back(partidas[numPartida].baraja.back());
                                        partidas[numPartida].baraja.pop_back();

                                        partidas[numPartida].manoJugador2 = calcularValorMano(partidas[numPartida].cartasJugador2);

                                        memset(buffer, 0, sizeof(buffer));

                                        // Enviar cartas al jugador 1
                                        snprintf(buffer, sizeof(buffer), "TUS-CARTAS:[%s,%d] [%s,%d]. OPONENTE:[%s,%d]",
                                                    partidas[numPartida].cartasJugador1[0].palo.c_str(), partidas[numPartida].cartasJugador1[0].numero,
                                                    partidas[numPartida].cartasJugador1[1].palo.c_str(), partidas[numPartida].cartasJugador1[1].numero,
                                                    partidas[numPartida].cartasJugador2[0].palo.c_str(), partidas[numPartida].cartasJugador2[0].numero);
                                        send(usuarios[partidas[numPartida].jugador1].id, buffer, strlen(buffer), 0);  // id de jugador1

                                        memset(buffer, 0, sizeof(buffer));

                                        // Enviar cartas al jugador 2
                                        snprintf(buffer, sizeof(buffer), "TUS-CARTAS:[%s,%d] [%s,%d]. OPONENTE:[%s,%d]",
                                                    partidas[numPartida].cartasJugador2[0].palo.c_str(), partidas[numPartida].cartasJugador2[0].numero,
                                                    partidas[numPartida].cartasJugador2[1].palo.c_str(), partidas[numPartida].cartasJugador2[1].numero,
                                                    partidas[numPartida].cartasJugador1[0].palo.c_str(), partidas[numPartida].cartasJugador1[0].numero);
                                        send(usuarios[partidas[numPartida].jugador2].id, buffer, strlen(buffer), 0);  // id de jugador2
                                    }
                                } else {
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde a Partida\n");
                                    send(i, buffer, strlen(buffer), 0);  // Cambiado a strlen(buffer)
                                }
                            }


                            if(strcmp(buffer, "PEDIR-CARTA\n") == 0){
                                int k = numUsuario(usuarios, i);
                                if(usuarios[k].estado == JUGANDO){
                                    int partUsuario = miPartida(partidas, i);
                                    if(whoAmI(partidas, i, partUsuario) == 1)
                                    {
                                        if(partidas[partUsuario].manoJugador1 > 21){
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "-Err. Excedido el valor de 21, si intenta solicitar más cartas una vez ya ha superado los 21 puntos, el servidor le devolverá error\n");
                                            send(i,buffer,sizeof(buffer),0);                
                                        } else {
                                            partidas[partUsuario].cartasJugador1.push_back(partidas[partUsuario].baraja.back());
                                            partidas[partUsuario].baraja.pop_back();

                                            partidas[partUsuario].manoJugador1 = calcularValorMano(partidas[partUsuario].cartasJugador1);
                                            int numCartasJugador = partidas[partUsuario].cartasJugador1.size() - 1; 

                                            memset(buffer, 0, sizeof(buffer));

                                            sprintf(buffer, "+Ok.[%s,%d] Tienes: %d",
                                                    partidas[partUsuario].cartasJugador1[numCartasJugador].palo.c_str(),
                                                    partidas[partUsuario].cartasJugador1[numCartasJugador].numero,
                                                    partidas[partUsuario].manoJugador1);
                                            send(partidas[partUsuario].jugador1, buffer, sizeof(buffer), 0);
                                        }
                                    } else {

                                        if(partidas[partUsuario].manoJugador2 > 21){
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "-Err. Excedido el valor de 21, si intenta solicitar más cartas una vez ya ha superado los 21 puntos, el servidor le devolverá error\n");
                                            send(i,buffer,sizeof(buffer),0);                
                                        } else {
                                            partidas[partUsuario].cartasJugador2.push_back(partidas[partUsuario].baraja.back());
                                            partidas[partUsuario].baraja.pop_back();

                                            partidas[partUsuario].manoJugador2 = calcularValorMano(partidas[partUsuario].cartasJugador2);
                                            int numCartasJugador = partidas[partUsuario].cartasJugador2.size() - 1; 

                                            memset(buffer, 0, sizeof(buffer));

                                            sprintf(buffer, "+Ok.[%s,%d] Tienes: %d",
                                                    partidas[partUsuario].cartasJugador2[numCartasJugador].palo.c_str(),
                                                    partidas[partUsuario].cartasJugador2[numCartasJugador].numero,
                                                    partidas[partUsuario].manoJugador2);
                                            send(partidas[partUsuario].jugador2, buffer, sizeof(buffer), 0);
                                        }
                                    }

                                } else {
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde a la partida\n");
                                    send(i,buffer,sizeof(buffer),0);        
                                }
                            }

                            if(strcmp(buffer, "PLANTARME\n") == 0){
                                int k = numUsuario(usuarios, i);
                                if(usuarios[k].estado == JUGANDO)
                                {
                                    usuarios[k].estado = WAITING;
                                    int partUsuario = miPartida(partidas, i);

                                    printf("[DEBUG] Partida: %d\n   > Jugador 1: %d\n   > Jugador 2: %d\n   > Estatus: %d\n", 
                                            partUsuario, partidas[partUsuario].jugador1, partidas[partUsuario].jugador2,
                                            partidas[partUsuario].estadoPartida);

                                    if(whoAmI(partidas, i, partUsuario) == 1){
                                        if(finPartida(partidas, usuarios, partUsuario)){
                                            int jug2 = numUsuario(usuarios, partidas[partUsuario].jugador2);
                                            usuarios[k].estado = USUARIO_VALIDADO;
                                            usuarios[jug2].estado = USUARIO_VALIDADO;
                                            resultadoPartida(partidas, partUsuario, usuarios);

                                        } else {
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Esperando al rival...\n");
                                            send(i,buffer,sizeof(buffer),0);    
                                        }
                                    } else {
                                        if(finPartida(partidas, usuarios, partUsuario)){
                                            int jug1 = numUsuario(usuarios, partidas[partUsuario].jugador1);
                                            usuarios[k].estado = USUARIO_VALIDADO;
                                            usuarios[jug1].estado = USUARIO_VALIDADO;
                                            resultadoPartida(partidas, partUsuario, usuarios);
                                        } else {
                                            memset(buffer, 0, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Esperando al rival...\n");
                                            send(i,buffer,sizeof(buffer),0);    
                                        }
                                    }
                                } else {
                                    //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde a Validacion\n");
                                    memset(buffer, 0, sizeof(buffer));
                                    strcpy(buffer, "-Err. El estado del usuario no corresponde a jugando\n");
                                    send(i,buffer,sizeof(buffer),0);
                                }
                            }

                            /* Salir */
                            if(strcmp(buffer, "SALIR\n") == 0) {
                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                                sacarUsuarioDesconectado(usuarios, i);
                            }
                        }

                        // Si el cliente introdujo ctrl+c
                        if(recibidos == 0) {
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                            /*
                              FUNCION PARA SACAR USUARIO DEL VECTOR
                            */
                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                            sacarUsuarioDesconectado(usuarios, i);
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