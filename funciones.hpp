#ifndef FUNCIONES_HPP
#define FUNCIONES_HPP

#include <vector>

bool existe_usuario(string &username, map<string, string> &userData);
void rellenarVectorUsuario(vector<string> &usernames, vector<string> &passwords);
void rellenarFicheroUsuario(map<string, string> &userData);

#endif