

if(strcmp(buffer, "PLANTARME\n") == 0){
    int k = numUsuario(usuarios, i);
    if(usuarios[k].estado == JUGANDO)
    {
        usuarios[k].estado = WAITING;
        int partUsuario = miPartida(partidas, i);
        if(whoAmI(partidas, i, partUsuario) == 1){
            if(finPartida(partidas, usuarios, i)){
                int jug2 = numUsuario(partidas[partUsuario].jugador2);
                usuarios[k].estado = USUARIO_VALIDADO;
                usuarios[jug2].estado = USUARIO_VALIDADO;
                resultadoPartida(partidas, partUsuario, usuarios);

            } else {
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "+Ok. Esperando al rival...\n");
                send(i,buffer,sizeof(buffer),0);    
            }
        } else {
            if(finPartida(partidas, usuarios, i)){
                int jug1 = numUsuario(partidas[partUsuario].jugador1);
                usuarios[k].estado = USUARIO_VALIDADO;
                usuarios[jug1].estado = USUARIO_VALIDADO;
                resultadoPartida(partidas, partUsuario, usuarios);
            } else {
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "+Ok. Esperando al rival...\n");
                send(i,buffer,sizeof(buffer),0);    
            }
        }
    } else {
        //send_to_user(i, buffer, sizeof(buffer), "-Err. El estado del usuario no corresponde a Validacion\n");
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "-Err. El estado del usuario no corresponde a jugando\n");
        send(i,buffer,sizeof(buffer),0);
    }
}