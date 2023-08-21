cmake_minimum_required(VERSION 3.0)

project(lighting)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(OpenGL_GL_PREFERENCE "GLVND")
endif()


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(my_program main.cpp)

target_link_libraries(my_program GLEW::GLEW glfw ${OPENGL_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
file(COPY ${CMAKE_SOURCE_DIR}/shader.f.glsl DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shader.v.glsl DESTINATION ${CMAKE_BINARY_DIR})