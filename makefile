CPP=g++
CPPFLAGS=-O3 -Wall -std=c++17 `sdl2-config --cflags`
LINK=-O3 `sdl2-config --libs` -pthread  -lSDL2_ttf
SRC= fonction.cpp main.cpp
OBJ=$(SRC:.cpp=.o)

%.o:	%.c
	$(CPP) -c $(CPPFLAGS) $< 

antcoloni: 	$(OBJ)
	$(CPP) $(OBJ) $(LINK) -o $@

clean:
	rm *.o antcoloni
