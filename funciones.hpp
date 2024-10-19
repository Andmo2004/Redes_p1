#ifndef FUNCIONES_HPP
#define FUNCIONES_HPP

#include <iostream>
#include <vector>
#include <map>
#include "bj_server.hpp"

using namespace std;

int numUsuario(const vector<Usuario> &usuarios, const int socket);
int miPartida(const vector<Mesa> &partidas, const int socket);
int buscarPartidaLibre(const vector<Mesa> &partidas);
int whoAmI(const vector<Mesa> &partidas, const int socket, const int partida);
int calcularValorMano(const std::vector<Carta>& mano);
bool existe_usuario(const string &username, const map<string, string> &userData);
bool contrasenia_correcta(const string &user, const string &password, const map<string, string> &userData);
bool usuarioIsConectado(const vector<Usuario> &usuarios, const string username);
bool buscar_palabra(const char *cadena, const char *palabra);
bool updateUserData(const string &username, const string &password, map<string, string> &userData);
void rellenarVectorUsuario(map<string, string> &userData);
void rellenarFicheroUsuario(const map<string, string> &userData);
void rellenarBaraja(vector<Carta> &baraja);
void mostrarUserData(const map<string, string> userData);
void sacarUsuarioDesconectado(vector<Usuario> &usuarios, const int socket);
void inicializarEstadoPartidas(vector<Mesa> &partidas);

#endif