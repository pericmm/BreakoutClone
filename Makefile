CC = g++
OBJECTS = main.o level.o pauseScreen.o lvlCreator.o
FLAGS = -lSDL2

all: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS)

%.o: %.c
	$(CC) -c $^


.PHONY: clean
clean: 
	rm -rf $(OBJECTS) a.out


