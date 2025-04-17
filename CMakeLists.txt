cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(LoadObj)

# Define o padrão do C++ como C++11 ou superior
set(CMAKE_CXX_STANDARD 20)

# Define o diretório de saída para os binários
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Adiciona o executável
add_executable(LoadObj src/main.cpp)

# Inclui os diretórios de cabeçalhos
include_directories(include)

# Configuração para Windows
if(WIN32)
    # Diretório da biblioteca e DLL
    set(FREEGLUT_LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
    set(FREEGLUT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
    set(FREEGLUT_DLL ${FREEGLUT_LIB_DIR}/freeglut.dll)

    # Inclui o diretório de cabeçalhos do freeglut
    include_directories(${FREEGLUT_INCLUDE_DIR})

    # Linka a biblioteca estática ou dinâmica
    target_link_libraries(LoadObj PRIVATE ${FREEGLUT_LIB_DIR}/freeglut.lib opengl32.lib)

    # Copia a DLL para o diretório de saída
    add_custom_command(TARGET LoadObj POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FREEGLUT_DLL} $<TARGET_FILE_DIR:LoadObj>)
else()
    message(FATAL_ERROR "Este CMakeLists.txt foi configurado apenas para Windows.")
endif()

# Define o projeto LoadObj como o projeto de inicialização
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LoadObj)