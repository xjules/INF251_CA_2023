# INF251_CA_2023
To host basic examples for INF251 course assignments 

# Linux instructions

 1. sudo apt install libgl-dev
 2. sudo apt install freeglut3-dev
 3. sudo apt install libglew-dev
 4. sudo apt install libgl1-mesa-dev
 5. sudo apt install libglfw3-dev
# Macos instructions

1. install brew
2. brew install cmake
3. brew install glew
4. brew install glfw3
5. xcode-select --install
6. test: 
    cc --version
    c++ --version


# Building and running instructions
1. cd <example_proj>
2. mkdir build
3. cd build
4. cmake ..
5. make
6. ./my_program