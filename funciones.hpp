#ifndef FUNCIONES_HPP
#define FUNCIONES_HPP

#include <iostream>
#include <vector>
#include <map>
#include "bj_server.hpp"

using namespace std;

bool existe_usuario(const string &username, const map<string, string> &userData);
bool contrasenia_correcta(const string &user, const string &password, const map<string, string> &userData);
bool usuarioIsConectado(const vector<Usuario> &usuarios, const string username);
bool buscar_palabra(const char *cadena, const char *palabra);
bool updateUserData(const string &username, const string &password, map<string, string> &userData);
void rellenarVectorUsuario(map<string, string> &userData);
void rellenarFicheroUsuario(const map<string, string> &userData);
void mostrarUserData(const map<string, string> userData);
void sacarUsuarioDesconectado(vector<Usuario> &usuarios, const int socket);

#endif