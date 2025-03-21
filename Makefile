CC = g++
OBJECTS = main.o level.o pauseScreen.o lvlCreator.o
FLAGS = -L/usr/include/SDL -lSDL2 -lSDL2_ttf 

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(FLAGS)

%.o: %.c
	$(CC) -c $^


.PHONY: clean
clean: 
	rm -rf $(OBJECTS) a.out


