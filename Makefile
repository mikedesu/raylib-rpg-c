CC=gcc
OBJ=-c
SHARED=-shared
LIBGAME_OBJECTS=gamestate.o 
STATIC_LINK_RAYLIB=-l:libraylib.a
LINK_MATH=-lm
POSITION_INDEPENDENT_CODE=-fPIC
MAIN_C=main.c

all: game 

#game: main.c libgame.so Gamestate.o
game: main.c gameloader.o gamestate.o libgame.so
	$(CC) -o $@ $(MAIN_C) gameloader.o gamestate.o $(POSITION_INDEPENDENT_CODE)  $(STATIC_LINK_RAYLIB) $(LINK_MATH)

gameloader.o: gameloader.c
	#$(CC) $(OBJ) $^ -o $@
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@

gamestate.o: gamestate.c
	#$(CC) $(OBJ) $^ -o $@
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE)  $^ -o $@
	#$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(STATIC_LINK_RAYLIB) $^ -o $@


libgame.o: libgame.c
	#$(CC) $(OBJ) $^ -o $@
	touch libgame.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@


libgame.so: libgame.o gamestate.o
	$(CC) $(SHARED) -o libgame.so libgame.o $(LIBGAME_OBJECTS) $(STATIC_LINK_RAYLIB)
	rm -rfv libgame.so.lockfile

clean:
	rm -rfv game *.o *.so *.lockfile
