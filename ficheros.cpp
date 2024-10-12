#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void rellenarFicheroUsuario(map<string, string> &userData){

    ofstream fich;

    fich.open("Users_Passwords.txt");

    for (const auto &entry : userData) {
        fich << entry.first << "\t" << entry.second << endl; // entry.first es el username y entry.second es el password
    }
    
    fich.close();
}

void rellenarVectorUsuario(vector<string> &usernames, vector<string> &passwords)
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

