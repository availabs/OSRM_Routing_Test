

OBJS = main.cpp src/Tile.cpp src/SimpleXmlWriter.cpp

CC = g++

COMPILER_FLAGS = -w -std=gnu++11

LINKER_FLAGS = -lSDL2 -lSDL2_image

OBJ_NAME = main

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)