#ifndef BJ_SERVER_H
#define BJ_SERVER_H

#include <string>
#include <vector>
#include <iostream>

void send_to_user(int i, char *buffer, size_t buffer_size, const char *msg);
void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);


using namespace std;

struct Usuario{
    /*
        Estado 0 = conectado
        Estado 1 = Usuario correcto
        Estado 2 = Usuario validado
        Estado 3 = Iniciar partida
        Estado 4 = Jugando
    */
    int estado; 
    string username;
    string password;
    int id;
};

struct Carta{
    string palo;
    int numero;
};

struct Mesa{
    int estadoPartida;
    vector<int> jugador;
    vector<struct Carta> baraja;
};

#endif