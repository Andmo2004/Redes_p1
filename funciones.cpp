#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <map>

#include "funciones.hpp"

using namespace std;

void rellenarFicheroUsuario(const map<string, string> &userData){

    ofstream fich;

    fich.open("Users_Passwords.txt");

    for (const auto &entry : userData) {
        fich << entry.first << "\t" << entry.second << endl; // entry.first es el username y entry.second es el password
    }
    
    fich.close();
}

void rellenarVectorUsuario(map<string, string> &userData)
{
    ifstream fich("Users_Passwords.txt");
    string username, password;

    if (!fich.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return;
    }

    // Leer los nombres de usuario y contraseñas desde el fichero
    while (fich >> username >> password) {
        userData[username] = password; // Almacenar en el map
    }

    fich.close();
}

bool existe_usuario(const string &username, const map<string, string> &userData)
{
    // find() para buscar el username en el map
    if (userData.find(username) != userData.end()) {
        return true;
    } else {
        return false;
    }
}

bool contrasenia_correcta(const string &user, const string &password, const map<string, string> &userData)
{
    auto it = userData.find(user);
    if (it != userData.end() && it->second == password) {
        return true;
    }
    return false;
}

bool buscar_palabra(const char *cadena, const char *palabra) {

    if (strstr(cadena, palabra) != NULL) {
        return true;
    }
    return false;
}

bool updateUserData(const string &username, const string &password, map<string, string> &userData)
{
    // Verificar si el usuario ya existe en el mapa
    if (userData.find(username) != userData.end()) {
        return false;
    } else {
        // Si el usuario no existe, lo agregamos al mapa
        userData[username] = password;
        return true; 
    }   
}