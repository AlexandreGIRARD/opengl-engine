CXX = g++

CXX_FILES =  main.cc src/matrix4.cc src/program.cc src/camera.cc src/mesh.cc src/model.cc src/glad.c src/stb_image.c
OBJ_FILES =   src/glad.o src/stb_image.o

CXX_FLAGS = -Wall -g -std=c++11 -I. -I./include
LDXX_FLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

MAIN_FILE = main.cc
EXEC = main

all:
	$(CXX)  $(CXX_FLAGS) -o $(EXEC) $(CXX_FILES) $(LDXX_FLAGS)
