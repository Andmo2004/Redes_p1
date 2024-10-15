while(1) {
    // Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
    auxfds = readfds;
    salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);

    if(salida > 0) {
        for(i = 0; i < FD_SETSIZE; i++) {
            // Buscamos el socket por el que se ha establecido la comunicación
            if(FD_ISSET(i, &auxfds)) {

                if(i == sd) { // Nueva conexión
                    if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1) {
                        perror("-Err. Error aceptando peticiones");
                    } else {
                        if(numClientes < MAX_CLIENTS) {
                            arrayClientes[numClientes] = new_sd;
                            FD_SET(new_sd, &readfds);
                            send_to_user(new_sd, buffer, sizeof(buffer), "+Ok. Usuario conectado\n");
                            usuarios[numClientes].estado = 0;
                            usuarios[numClientes].id = new_sd;
                            numClientes++;
                            
                            for(j = 0; j < (numClientes - 1); j++) {
                                sprintf(buffer, "+Ok. Nuevo Cliente conectado en <%d>", new_sd);
                                send_to_user(arrayClientes[j], buffer, sizeof(buffer), buffer);
                            }
                        } else {
                            send_to_user(new_sd, buffer, sizeof(buffer), "-Err. Demasiados clientes conectados\n");
                            close(new_sd);
                        }
                    }
                } else if (i == 0) { // Información de teclado
                    bzero(buffer, sizeof(buffer));
                    fgets(buffer, sizeof(buffer), stdin);
                    
                    if(strcmp(buffer, "SALIR\n") == 0) {
                        for(j = 0; j < numClientes; j++) {
                            send_to_user(arrayClientes[j], buffer, sizeof(buffer), "+Ok. Desconexión servidor\n");
                            close(arrayClientes[j]);
                            FD_CLR(arrayClientes[j], &readfds);
                        }
                        close(sd);
                        exit(-1);
                    }
                    // Implementar envío de mensajes a los clientes
                } else { // Manejo de clientes conectados
                    bzero(buffer, sizeof(buffer));
                    recibidos = recv(i, buffer, sizeof(buffer), 0);
                    
                    if(recibidos > 0) {
                        /* Inicio de sesión */
                        if(buscar_palabra(buffer, "USUARIO")) {
                            if(sscanf(buffer, "%s %s", temp, username) == 2) {
                                if(!existe_usuario(string(username), userData)) {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario no existente \n");
                                    salirCliente(i, &readfds, &numClientes, arrayClientes);
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "-Ok. Usuario correcto\n");
                                    for(int w = 0; w < usuarios.size(); w++) {
                                        if(usuarios[w].id == i) {
                                            usuarios[w].estado = 1;
                                        }
                                    }
                                }
                            }
                        }

                        if(buscar_palabra(buffer, "PASSWORD")) {
                            if(sscanf(buffer, "%s %s", temp, password) == 2) {
                                if(!contrasenia_correcta(string(username), string(password), userData)) {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Contrasenia incorrecta\n");
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario validado\n");
                                    for(int w = 0; w < usuarios.size(); w++) {
                                        if(usuarios[w].id == i) {
                                            usuarios[w].estado = 2;
                                        }
                                    }
                                }
                            }
                        }

                        /* Iniciar partida */
                        if(buscar_palabra(buffer, "INICIAR-PARTIDA")) {
                            int counter = 0;
                            for(int w = 0; w < usuarios.size(); w++) {
                                if(usuarios[w].id == i) {
                                    usuarios[w].estado = 3;
                                }
                            }
                            for(int w = 0; w < usuarios.size(); w++) {
                                if(usuarios[w].estado == 3) {
                                    counter++;
                                    if(counter == 2) {
                                        counter = 0;
                                    } else {
                                        send_to_user(usuarios[w].id, buffer, sizeof(buffer), "+Ok. Esperando a otro jugador...\n");
                                    }
                                }
                            }
                        }

                        /* Registro */
                        if(buscar_palabra(buffer, "REGISTRO")) {
                            if(sscanf(buffer, "REGISTRO -u %s -p %s", username, password) == 2) {
                                if(updateUserData(string(username), string(password), userData)) {
                                    send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario registrado correctamente, sal e inicia sesion\n");
                                    for(int w = 0; w < usuarios.size(); w++) {
                                        if(usuarios[w].id == i) {
                                            usuarios[w].username = username;
                                            usuarios[w].password = password;
                                        }
                                    }
                                    salirCliente(i, &readfds, &numClientes, arrayClientes);
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. El usuario ya esta registrado\n");
                                    salirCliente(i, &readfds, &numClientes, arrayClientes);
                                }
                            } else {
                                send_to_user(i, buffer, sizeof(buffer), "-Err. La informacion no se ha introducido correctamente\n");
                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                            }
                        }

                        /* Salir */
                        if(buscar_palabra(buffer, "SALIR")) {
                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                        }
                    }

                    if(strcmp(buffer, "SALIR\n") == 0) {
                        salirCliente(i, &readfds, &numClientes, arrayClientes);
                    } else {
                        snprintf(identificador, sizeof(identificador), "<%d>: %.240s", i, buffer);
                        bzero(buffer, sizeof(buffer));
                        strcpy(buffer, identificador);
                        printf("%s\n", buffer);

                        for(j = 0; j < numClientes; j++) {
                            if(arrayClientes[j] != i) {
                                send(arrayClientes[j], buffer, sizeof(buffer), 0);
                            }
                        }
                    }

                    // Si el cliente introdujo ctrl+c
                    if(recibidos == 0) {
                        printf("El socket %d, ha introducido ctrl+c\n", i);
                        salirCliente(i, &readfds, &numClientes, arrayClientes);
                    }
                }
            }
        }
    }
    
    close(sd);
    return 0;
}
