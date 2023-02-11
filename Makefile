CC = gcc
CFLAGS = -Wall -g -O3 -std=c99
LDLIBS = -lallegro `pkg-config --libs $(AL_LIBS)` -lm
AL_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5
SRC = $(wildcard sources/*.c)
OBJ = $(SRC:.c=.o)
TARGET = jewels

all: $(TARGET)

$(TARGET): $(OBJ)

run: all
	./$(TARGET)

restart:
	rm resources/score/score_history.txt
	rm resources/mission/mission_history.txt
	rm resources/board/board_history.txt

clean:
	-rm -f sources/*.o *~f

purge: clean
	-rm -f $(TARGET)

