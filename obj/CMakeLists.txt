cmake_minimum_required(VERSION 3.0)

project(obj)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(OpenGL_GL_PREFERENCE "GLVND")
endif()
find_package(GLUT REQUIRED)
if(GLUT_FOUND)
  message(STATUS "INC_DIR= Found GLUT: ${GLUT_INCLUDE_DIRS}")
  message(STATUS "LIB= Found GLUT: ${GLUT_LIBRARIES}")
endif()

find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
  message(STATUS "INC_DIR=Found OpenGL: ${OPENGL_INCLUDE_DIR}")
  message(STATUS "LIB= Found OpenGL: ${OPENGL_LIBRARIES}")
endif()

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  message(STATUS "INC_DIR=Found GLEW: ${GLEW_INCLUDE_DIRS}")
  message(STATUS "LIB= Found GLEW: GLEW::GLEW")
endif()

set(SOURCES
    main.cpp
    model_obj.cpp
)
add_executable(my_program ${SOURCES})

target_link_libraries(my_program GLEW::GLEW ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
file(COPY ${CMAKE_SOURCE_DIR}/shader.f.glsl DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shader.v.glsl DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/capsule DESTINATION ${CMAKE_BINARY_DIR})