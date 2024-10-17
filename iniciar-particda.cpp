void iniciarPartida(int socket, std::vector<Usuario>& usuarios, std::vector<Mesa>& partidas) {
    int k = numUsuario(usuarios, socket); // Encuentra el usuario que envió la petición
    if (usuarios[k].estado == USUARIO_VALIDADO) {
        // Busca una mesa con solo un jugador esperando o crea una nueva
        for (Mesa& mesa : partidas) {
            if (mesa.estadoPartida == 0 && mesa.jugador1 == -1) {
                mesa.jugador1 = socket;
                usuarios[k].estado = INICIAR_PARTIDA;
                send(socket, "+Ok. Esperando otro jugador\n", sizeof("+Ok. Esperando otro jugador\n"), 0);
                return;
            } else if (mesa.estadoPartida == 0 && mesa.jugador2 == -1) {
                mesa.jugador2 = socket;
                mesa.estadoPartida = 1; // Comienza la partida
                usuarios[k].estado = INICIAR_PARTIDA;
                
                // Barajamos y repartimos
                mesa.baraja = crearBaraja();
                mesa.cartasJugador1.push_back(mesa.baraja.back());
                mesa.baraja.pop_back();
                mesa.cartasJugador1.push_back(mesa.baraja.back());
                mesa.baraja.pop_back();
                
                mesa.cartasJugador2.push_back(mesa.baraja.back());
                mesa.baraja.pop_back();
                mesa.cartasJugador2.push_back(mesa.baraja.back());
                mesa.baraja.pop_back();

                // Enviamos la información de las cartas a ambos jugadores
                char buffer[250];
                sprintf(buffer, "+Ok. Empieza la partida. TUS-CARTAS:[%s,%d] [%s,%d]. OPONENTE:[%s,%d]",
                        mesa.cartasJugador1[0].palo.c_str(), mesa.cartasJugador1[0].numero,
                        mesa.cartasJugador1[1].palo.c_str(), mesa.cartasJugador1[1].numero,
                        mesa.cartasJugador2[0].palo.c_str(), mesa.cartasJugador2[0].numero);

                send(mesa.jugador1, buffer, sizeof(buffer), 0);

                sprintf(buffer, "+Ok. Empieza la partida. TUS-CARTAS:[%s,%d] [%s,%d]. OPONENTE:[%s,%d]",
                        mesa.cartasJugador2[0].palo.c_str(), mesa.cartasJugador2[0].numero,
                        mesa.cartasJugador2[1].palo.c_str(), mesa.cartasJugador2[1].numero,
                        mesa.cartasJugador1[0].palo.c_str(), mesa.cartasJugador1[0].numero);

                send(mesa.jugador2, buffer, sizeof(buffer), 0);
                return;
            }
        }
    } else {
        char buffer[250];
        sprintf(buffer, "-Err. El estado del usuario no corresponde a Validacion\n");
        send(socket, buffer, sizeof(buffer), 0);
    }
}