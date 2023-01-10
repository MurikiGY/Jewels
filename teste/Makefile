CC = gcc
CFLAGS = -Wall -g -O3 -std=c99
LDLIBS = -lallegro `pkg-config --libs $(AL_LIBS)` -lm
AL_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = game

all: $(TARGET)

$(TARGET): $(OBJ)

run: all
	./$(TARGET)

clean:
	-rm -f *.o *~f

purge: clean
	-rm -f $(TARGET)

