OBJECTS=tile_process.o \
	src/cpp/Tile.o \
	src/cpp/SimpleXmlWriter.o \
	src/cpp/ArgParser.o \
	src/cpp/GeojsonWriter.o \
	src/cpp/PNG_Image.o \
	src/cpp/utils.o

#OBJECTS=$(SOURCES:.cpp=.o)

CC=g++

CFLAGS=-Wall -c -std=gnu++11

LIBS=-lpng16

EXECUTABLE=tile_process

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ $(LIBS) -o $@

tile_process.o: src/cpp/SimpleXmlWriter.h src/cpp/Tile.h src/cpp/GeojsonWriter.h src/cpp/utils.h src/cpp/ArgParser.h

src/cpp/Tile.o: src/cpp/Tile.h

src/cpp/SimpleXmlWriter.o: src/cpp/SimpleXmlWriter.h

src/cpp/ArgParser.o: src/cpp/ArgParser.h

src/cpp/GeojsonWriter.o: src/cpp/GeojsonWriter.h src/cpp/Tile.h

src/cpp/PNG_Image.o: src/cpp/PNG_Image.h

src/cpp/utils.o: src/cpp/utils.h

%.o: %.cpp
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -rf src/cpp/*.o