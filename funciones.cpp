#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <map>
#include <algorithm>

#include <time.h>

#include "funciones.hpp"
#include "bj_server.hpp"

using namespace std;

void rellenarFicheroUsuario(const map<string, string> &userData){

    ofstream fich;

    fich.open("Users_Passwords.txt");

    for (const auto &entry : userData) {
        fich << entry.first << "\t" << entry.second<<"\n"; // entry.first es el username y entry.second es el password
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

    if (strncmp(cadena,palabra,strlen(palabra))==0) {
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
        rellenarFicheroUsuario(userData);
        return true; 
    }   
}



void rellenarBaraja(vector<Carta> &baraja)
{
    // Puedes usar un array de strings para los palos
    string palos[] = {"Corazones", "Treboles", "Picas", "Diamantes"};
    
    // Reservar el tamaño necesario (4 palos x 13 cartas)
    baraja.reserve(52); 
    
    for(int i = 0; i < 4; i++) {  // Para los 4 palos
        for(int j = 0; j < 13; j++) {  // Para las 13 cartas de cada palo
            Carta nuevaCarta;
            nuevaCarta.numero = j + 1;   // Número de la carta
            nuevaCarta.palo = palos[i];  // Asignar palo
            
            // Agregar la carta al vector
            baraja.push_back(nuevaCarta);
        }
    }
    random_shuffle(baraja.begin(), baraja.end());
}

void mostrarUserData(const map<string, string> userData)
{    
    auto it = userData.begin();
    for (int i = 0; it != userData.end() && i < userData.size(); ++i, ++it)
    {
        /*  [DEBUG] BIEN  */ 
        cout << "Usuario " << i + 1 << ": " << it->first << "\n"
            << "Contraseña: " << it->second << endl; 
    }
}

void sacarUsuarioDesconectado(vector<Usuario> &usuarios, const int socket)
{
    for(size_t i = 0; i < usuarios.size(); ++i)
    {
        if(usuarios[i].id == socket)
        {
            // Eliminar el usuario del vector
            cout << "Usuario " << usuarios[i].username << " ha sido eliminado." << endl;
            usuarios.erase(usuarios.begin() + i);
            return; // Terminamos una vez eliminado
        }
    }
    cout << "Usuario con socket " << socket << " no encontrado." << endl;
}

bool usuarioIsConectado(const vector<Usuario> &usuarios, const string username)
{
    cout << "Usuarios conectados" << endl;
    for(size_t i = 0; i < usuarios.size(); ++i)
    {
        cout << usuarios[i].username << endl;
        if(usuarios[i].username == username)
        {
            
            return true; // Terminamos una vez eliminado
        }   
    }
    return false;
}

int numUsuario(const vector<Usuario> &usuarios, const int socket)
{
    for(int i=0; i<usuarios.size(); ++i)
    {
        if(usuarios[i].id == socket) return i;
    }

    return 0;
}

// Función para calcular el valor de una mano
int calcularValorMano(const std::vector<Carta>& mano) {
    int valorTotal = 0;
    int ases = 0;

    for (const Carta& carta : mano) {
        if (carta.numero >= 2 && carta.numero <= 10) {
            valorTotal += carta.numero;
        } else if (carta.numero >= 11 && carta.numero <= 13) {
            valorTotal += 10; // J, Q, K valen 10
        } else if (carta.numero == 1) {
            ases++;
            valorTotal += 11; // El As inicialmente vale 11
        }
    }

    // Ajustar el valor del As si es necesario
    while (valorTotal > 21 && ases > 0) {
        valorTotal -= 10; // El As puede valer 1 en lugar de 11
        ases--;
    }

    return valorTotal;
}

int buscarPartidaLibre(const vector<Mesa> &partidas)
{
    for(int i=0; i<partidas.size(); ++i)
    {
        if(partidas[i].estadoPartida == VACIA || partidas[i].estadoPartida == INCOMPLETA){
            return i;
        }
    }
    return 0;
}

void inicializarEstadoPartidas(vector<Mesa> &partidas)
{
    for(int i=0; i<partidas.size(); ++i)
    {
        partidas[i].estadoPartida = VACIA;
    }

}

int miPartida(const vector<Mesa> &partidas, const int socket)
{
    for(int i=0; i<partidas.size(); ++i)
    {
        if((partidas[i].jugador1 == socket) || (partidas[i].jugador2 == socket)){
            return i;
        }
    }
    return 0;
}