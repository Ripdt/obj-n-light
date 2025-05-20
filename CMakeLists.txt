cmake_minimum_required(VERSION 3.10)

project(LoadObj)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(LoadObj src/main.cpp)

include_directories(include)

if(WIN32)
    # Windows
    set(FREEGLUT_LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
    set(FREEGLUT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
    set(FREEGLUT_DLL ${FREEGLUT_LIB_DIR}/freeglut.dll)

    include_directories(${FREEGLUT_INCLUDE_DIR})

    target_link_libraries(LoadObj PRIVATE ${FREEGLUT_LIB_DIR}/freeglut.lib opengl32.lib)

		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LoadObj)

    add_custom_command(TARGET LoadObj POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FREEGLUT_DLL} $<TARGET_FILE_DIR:LoadObj>)
elseif(APPLE)
    # macOS usa os frameworks do sistema
    target_link_libraries(LoadObj PRIVATE "-framework OpenGL" "-framework GLUT")
else()
    message(FATAL_ERROR "Plataforma não suportada.")
endif()
