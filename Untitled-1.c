
                            int num=0,num2=0;
                            char username[50], temp[50];

                            bzero(buffer,sizeof(buffer));
                            
                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                            if(recibidos > 0){

                                if(buscar_palabra(buffer, "USUARIO")){

                                    // comprobamos que haya introducido un nombre de usuario
                                    if (sscanf(buffer, "%s %s", temp, username) == 2){

                                        if(!existe_usuario(usuario, arrayClientes, numClientes)){
                                            send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario no existente \n");
                                            salirCliente(i,&readfds,&numClientes,arrayClientes);

                                        } else {
                                            int k=0;
                                            while(k<3){
                                                send_to_user(i, buffer, sizeof(buffer), " >> Introduzca su contraseniaa: PASSWORD password \n");
                                                bzero(buffer, sizeof(buffer));
                                                recv(i, buffer, sizeof(buffer), 0);
                                                
                                                if(buscar_palabra(buffer, "PASSWORD")){
                                                    /*
                                                        buscar contraseña en la estructura
                                                    */
                                                    if(!){
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



                                    } else {
                                        send_to_user(i, buffer, sizeof(buffer), "-Err. Nombre de usuario no introducido \n");
                                        salirCliente(i,&readfds,&numClientes,arrayClientes);
                                    }
                                    /*
                                    */
                                } 
                                

                                else{
                                    
                                    if(num==i){

                                        send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario correcto\n");

                                        bzero(buffer,sizeof(buffer));

                                        recibidos = recv(i,buffer,sizeof(buffer),0);

                                        if(recibidos>0){

                                            if(sscanf(buffer,"PASSWORD %d",&num2)!=1){

                                                send_to_user(i, buffer, sizeof(buffer), "-Err. Error en la validación\n");
                                            }
                                            else{

                                                if(num2==i){
                                                    
                                                    send_to_user(i, buffer, sizeof(buffer), "+Ok. Usuario validado\n");

                                                    bzero(buffer,sizeof(buffer));
                                                    recibidos = recv(i,buffer,sizeof(buffer),0);

                                                }else{
                                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Error en la validación\n");
                                                }
                                            }
                                        }
                                    }
                                    else{
                                        
                                        send_to_user(i, buffer, sizeof(buffer), "-Err. Usuario incorrecto\n");
                                    }

                                }

                                if(sscanf(buffer,"REGISTRO -u %d -p %d",&num,&num2)!=2){

                                    send_to_user(i, buffer, sizeof(buffer), "-Err. Error en registro\n");
                                }
                            }


                            /*
                                INICIAR PARTIDA
                            */


bool buscar_palabra(const char *cadena, const char *palabra) {

    if (strstr(cadena, palabra) != NULL) {
        return true;
    }
    return false;
}

bool existe_usuario(const char* usuario, const char** arrayClientes, size_t numClientes)
{
    for(size_t i=0; i < numClientes; ++i)
    {
        if(strcmp(usuario, arrayClientes[i])){
            return true;
        }
    }
    return false;
}

int main(){

    
}