	    while (1) {
        auxfds = readfds; // Copiamos readfds en auxfds para usarlo en select()
        int salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);
        
        if (salida == -1) {
            perror("Error en select");
            exit(EXIT_FAILURE);
        }

        if (salida > 0) {
            for (int i = 0; i < FD_SETSIZE; i++) {
                if (FD_ISSET(i, &auxfds)) {
                    if (i == sd) { // Nueva conexión
                        from_len = sizeof(from);
                        new_sd = accept(sd, (struct sockaddr*)&from, &from_len);
                        if (new_sd == -1) {
                            perror("-Err. Error aceptando peticiones");
                        } else {
                            if (numClientes < MAX_CLIENTS) {
                                // Añadir nuevo cliente
                                arrayClientes[numClientes] = new_sd;
                                usuarios[numClientes].estado = CONECTADO;
                                usuarios[numClientes].id = new_sd;
                                numClientes++;
                                
                                FD_SET(new_sd, &readfds);
                                send_to_user(new_sd, buffer, sizeof(buffer), "+Ok. Usuario conectado\n");
                                printf("Nuevo cliente conectado: %d\n", new_sd);
                            } else {
                                send_to_user(new_sd, buffer, sizeof(buffer), "-Err. Demasiados clientes conectados\n");
                                close(new_sd);
                            }
                        }
                    } else if (i == 0) { // Entrada desde teclado
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);

                        if (strcmp(buffer, "SALIR\n") == 0) {
                            for (int j = 0; j < numClientes; j++) {
                                send_to_user(arrayClientes[j], buffer, sizeof(buffer), "+Ok. Desconexión servidor\n");
                                close(arrayClientes[j]);
                                FD_CLR(arrayClientes[j], &readfds);
                            }
                            close(sd);
                            exit(0);
                        }

                    } else { // Manejo de clientes conectados
                        bzero(buffer, sizeof(buffer));
                        int recibidos = recv(i, buffer, sizeof(buffer), 0);
                        
                        if (recibidos > 0) {
                            // Procesar mensaje del cliente
                            if (buscar_palabra(buffer, "REGISTRO")) {
                                if (usuarios[i].estado == CONECTADO) {
                                    char username[50], password[50];
                                    if (sscanf(buffer, "REGISTRO -u %s -p %s", username, password) == 2) {
                                        if (updateUserData(string(username), string(password), userData)) {
                                            send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario registrado correctamente\n");
                                            userData[string(username)] = string(password); // Añadir al mapa
                                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                                        } else {
                                            send_to_user(i, buffer, sizeof(buffer), "-Err. El usuario ya está registrado\n");
                                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                                        }
                                    } else {
                                        send_to_user(i, buffer, sizeof(buffer), "-Err. Información incorrecta\n");
                                        salirCliente(i, &readfds, &numClientes, arrayClientes);
                                    }
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Estado no permite registro\n");
                                }
                            }

                            if (buscar_palabra(buffer, "USUARIO")) {
                                if (usuarios[i].estado == CONECTADO) {
                                    char temp[50], username[50];
                                    if (sscanf(buffer, "%s %s", temp, username) == 2) {
                                        if (!existe_usuario(string(username), userData)) {
                                            send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario inexistente\n");
                                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                                        } else {
                                            send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario correcto\n");
                                            usuarios[i].estado = USUARIO_CORRECTO;
                                            printf("Usuario %s validado, estado: %d\n", username, usuarios[i].estado);
                                        }
                                    }
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Estado incorrecto\n");
                                }
                            }

                            if (buscar_palabra(buffer, "PASSWORD")) {
                                if (usuarios[i].estado == USUARIO_CORRECTO) {
                                    char temp[50], password[50];
                                    if (sscanf(buffer, "%s %s", temp, password) == 2) {
                                        if (!contrasenia_correcta(temporal, string(password), userData)) {
                                            send_to_user(i, buffer, sizeof(buffer), "-Err. Contraseña incorrecta\n");
                                        } else {
                                            send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario validado\n");
                                            usuarios[i].estado = USUARIO_VALIDADO;
                                            printf("Contraseña correcta, estado: %d\n", usuarios[i].estado);
                                        }
                                    }
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Estado incorrecto para contraseña\n");
                                }
                            }

                            if (buscar_palabra(buffer, "INICIAR-PARTIDA")) {
                                if (usuarios[i].estado == USUARIO_VALIDADO) {
                                    usuarios[i].estado = 3; // Estado de partida
                                    printf("Iniciando partida, estado: %d\n", usuarios[i].estado);
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Estado no permite iniciar partida\n");
                                }
                            }

                            if (strcmp(buffer, "SALIR\n") == 0) {
                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                            }
                        }

                        // Cliente desconectado
                        if (recibidos == 0) {
                            printf("Cliente %d desconectado (ctrl+c)\n", i);
                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                        }
                    }
                }
            }
        }
    }
    
    close(sd);
    return 0;
}