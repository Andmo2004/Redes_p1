#include <iostream>
std::map<string, string> userData;
                            
                            char username[50], password[50], temp[50];

                            bzero(buffer,sizeof(buffer));
                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                            if(recibidos > 0)
                            {
                                /*+++++++++++++++++++++++++++++++ INICIO DE SESION +++++++++++++++++++++++++++++++*/

                                if(buscar_palabra(buffer, "USUARIO")){

                                    // comprobamos que haya introducido un nombre de usuario
                                    if (sscanf(buffer, "%s %s", temp, username) == 2){
                                        if(!existe_usuario(std::string(username), userData)){
                                            send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario no existente \n");
                                            salirCliente(i,&readfds,&numClientes,arrayClientes);

                                        } else {
                                            int k=0;
                                            while(k<3){
                                                send_to_user(i, buffer, sizeof(buffer), " >> Introduzca su contrasenia: PASSWORD password \n");
                                                bzero(buffer, sizeof(buffer));
                                                recv(i, buffer, sizeof(buffer), 0);
                                                
                                                if(buscar_palabra(buffer, "PASSWORD")){
                                                    if (sscanf(buffer, "%s %s", temp, password) == 2){
                                                        if(!contrasenia_correcta(std::string(username), std::string(password), userData)){
                                                            send_to_user(i, buffer, sizeof(buffer), "Contrasenia incorrecta tienes 3 intentos \n");
                                                            k++;
                                                            if(k == 3){
                                                                send_to_user(i, buffer, sizeof(buffer), "-Err. Has metido la contrasenia 3 veces mal \n");
                                                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                                                            }

                                                        } else {
                                                            send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario validado\n");
                                                            break;
                                                        }
                                                    }
                                                }
                                            }
                                        }


                                    } else {
                                        send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario no introducido \n");
                                        salirCliente(i,&readfds,&numClientes,arrayClientes);
                                    }
                                
                                /*+++++++++++++++++++++++++++++++ REGISTRO +++++++++++++++++++++++++++++++*/
                                
                                } if (buscar_palabra(buffer, "REGISTRO")) {
                                    if(sscanf(buffer,"REGISTRO -u %s -p %s",&username,&password)==2){
                                        if(updateUserData(std::string(username), std::string(password), userData)){
                                            send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario registrado correctamente, sal e inicia sesion\n");
                                            salirCliente(i,&readfds,&numClientes,arrayClientes);
                                        } else {
                                            send_to_user(i, buffer, sizeof(buffer), "-Err. El usuario ya esta registrado\n");
                                            salirCliente(i,&readfds,&numClientes,arrayClientes);
                                        }
                                    } else {
                                        send_to_user(i, buffer, sizeof(buffer), "-Err. La informacion no se ha introducido correctamente\n");
                                        salirCliente(i,&readfds,&numClientes,arrayClientes);
                                    }
                                } if (buscar_palabra(buffer, "SALIR")) {
                                    salirCliente(i,&readfds,&numClientes,arrayClientes); 
                                } else {
                                    send_to_user(i, buffer, sizeof(buffer), "-Err. El usuario ya esta registrado\n");
                                }
                                
                                /*
                                    INICIAR PARTIDA
                                */
                            }




