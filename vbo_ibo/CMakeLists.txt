cmake_minimum_required(VERSION 3.0)

project(VboIbo)

set(GL_INCLUDE_DIRS "/home/xjules/.local/py310-inf251/include")

add_executable(my_app
    main.cpp
    # Add other source files here
)


# Set the path to GLEW and GLUT libraries (adjust paths as needed)
set(GLUT_LIBRARIES "/home/xjules/.local/py310-inf251/lib/libglut.a")
set(GLEW_LIBRARIES "/home/xjules/.local/py310-inf251/lib/libGLEW.a")

# Include directories
target_include_directories(my_app PRIVATE ${GL_INCLUDE_DIRS})

# Link libraries
target_link_libraries(my_app PRIVATE ${GLEW_LIBRARIES} ${GLUT_LIBRARIES})