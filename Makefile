

OBJS = main.cpp \
	src/cpp/Tile.cpp \
	src/cpp/SimpleXmlWriter.cpp \
	src/cpp/PNG_Image.cpp \
	src/cpp/utils.cpp

CC = g++

COMPILER_FLAGS = -w -std=gnu++11

LINKER_FLAGS = -lpng16

OBJ_NAME = tile_process

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)