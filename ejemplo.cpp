

if(strcmp(buffer, "INICIAR-PARTIDA\n") == 0){
    int k = numUsuario(usuarios, i);
    if(usuarios[k].estado == USUARIO_VALIDADO){
        usuarios[k].estado = INICIAR_PARTIDA;
        printf("Iniciando partida, estado: %d\n", usuarios[k].estado);
        
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "+Ok. Entrando en la cola para jugar...\n");
        send(i,buffer,sizeof(buffer),0);      

        int numPartida = buscarPartidaLibre(partidas);

        if(partidas[numPartida].estado == VACIA){
            partidas[numPartida].jugador1 = k;
            partidas[numPartida].estado = INCOMPLETA;
            
            usuarios[k].estado = WAITING;

            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "+Ok. Esperando otro jugador\n");
            send(k,buffer,sizeof(buffer),0);   
    
        } 
        if(partidas[numPartida].estado == INCOMPLETA){

            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "+Ok. Empieza la partida.\n");
            send(k,buffer,sizeof(buffer),0);   

            partidas[numPartida].jugador2 = k;
            partidas[numPartida].estado = EMPEZADA;

            usuarios[partidas[numPartida].jugador1].estado = JUGANDO;
            usuarios[partidas[numPartida].jugador2].estado = JUGANDO;

            vector<Carta> barajaPartida;
            rellenarBaraja(barajaPartida);

            partidas[numPartida].baraja = barajaPartida;

            /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
            
            // DAMOS DOS CARTAS AL JUGADOR 1
            partidas[numPartida].cartasJugador1.push_back(partidas[numPartida].baraja.back());
            partidas[numPartida].baraja.pop_back();

            partidas[numPartida].cartasJugador1.push_back(partidas[numPartida].baraja.back());
            partidas[numPartida].baraja.pop_back();
            
            // DAMOS DOS CARTAS AL JUGADOR 2
            partidas[numPartida].cartasJugador2.push_back(partidas[numPartida].baraja.back());
            partidas[numPartida].baraja.pop_back();

            partidas[numPartida].cartasJugador2.push_back(partidas[numPartida].baraja.back());
            partidas[numPartida].baraja.pop_back();
        
            memset(buffer, 0, sizeof(buffer));

            sprintf(buffer, "TUS-CARTAS:[%s,%d] [%s,%d]. OPONENTE:[%s,%d]",
                        partidas[numPartida].cartasJugador1[0].palo.c_str(), partidas[numPartida].cartasJugador1[0].numero,
                        partidas[numPartida].cartasJugador1[1].palo.c_str(), partidas[numPartida].cartasJugador1[1].numero,
                        partidas[numPartida].cartasJugador2[0].palo.c_str(), partidas[numPartida].cartasJugador2[0].numero);
            send(partidas[numPartida].jugador1, buffer, sizeof(buffer), 0);

            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "TUS-CARTAS:[%s,%d] [%s,%d]. OPONENTE:[%s,%d]",
                        partidas[numPartida].cartasJugador2[0].palo.c_str(), partidas[numPartida].cartasJugador2[0].numero,
                        partidas[numPartida].cartasJugador2[1].palo.c_str(), partidas[numPartida].cartasJugador2[1].numero,
                        partidas[numPartida].cartasJugador1[0].palo.c_str(), partidas[numPartida].cartasJugador1[0].numero);

            send(partidas[numPartida].jugador2, buffer, sizeof(buffer), 0);     
        }
    } else {
        //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde a Validacion\n");
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "-Err. El estado del usuario no corresponde a Partida\n");
        send(i,buffer,sizeof(buffer),0);
    }
}

if(strcmp(buffer, "PEDIR-CARTA\n") == 0){
    int k = numUsuario(usuarios, i);
    if(usuarios[k].estado == JUGANDO){
        int partUsuario == miPartida(partidas, i);
        
        if(partidas[partUsuario].manoJugador1)



    } else {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "-Err. El estado del usuario no corresponde a la partida\n");
        send(i,buffer,sizeof(buffer),0);        
    }
}