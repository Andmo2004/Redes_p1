cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(redes_bj)

# Establecer la versión de C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Añadir los ejecutables y las fuentes
add_executable(server 
    bj_server.cpp
    funciones.cpp
)

add_executable(client
    bj_client2.cpp
)

# Incluir directorios para los encabezados
include_directories(
    .
)

# Opciones de compilación (añadir warnings y optimización)
target_compile_options(server PRIVATE -Wall -O2)
target_compile_options(client PRIVATE -Wall -O2)
