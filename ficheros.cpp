#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void rellenaVectorUsuario(vector<string> &usernames, vector<string> &passwords){

    ofstream fich;

    fich.open("Users_Passwords.txt");

    for(int i=0;i<usernames.size();i++){

        fich<<usernames[i]<<"\t"<<passwords[i]<<endl;
    }
    
    fich.close();
}