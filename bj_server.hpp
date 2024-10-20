#ifndef BJ_SERVER_H
#define BJ_SERVER_H

#include <string>
#include <vector>
#include <iostream>

//void send_to_user(int i, char *buffer, size_t buffer_size, const char *msg);
void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);


using namespace std;

/*
    Estado 0 = conectado
    Estado 1 = Usuario correcto
    Estado 2 = Usuario validado
    Estado 3 = Iniciar partida
    Estado 4 = Jugando
    Estado 5 = Waiting
*/
const int CONECTADO=0;

const int USUARIO_CORRECTO=1;
const int USUARIO_VALIDADO=2;

const int INICIAR_PARTIDA =3;
const int JUGANDO=4;
const int WAITING=5;
const int FINALIZADO=6;

struct Usuario{

    int estado; 
    string username;
    string password;
    int id;
};

struct Carta{
    string palo;
    int numero;
};

/*
    Estado 0: Incompleta
    Estado 1: Jugando
*/

const int VACIA=0;
const int INCOMPLETA=1;
const int EMPEZADA=2;

struct Mesa{
    int estadoPartida;
    int jugador1;
    int jugador2;
    int manoJugador1;
    int manoJugador2;
    vector<Carta> baraja;
    vector<Carta> cartasJugador1;
    vector<Carta> cartasJugador2;
};

void sendError(int userId, const char* message);
void sendMessage(int userId, const char* message);

#endif