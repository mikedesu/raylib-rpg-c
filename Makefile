CC=gcc
OBJ=-c
SHARED=-shared
DATA_STRUCTS=vectorentityid.o 
LIBGAME_OBJECTS=gamestate.o $(DATA_STRUCTS)
STATIC_LINK_RAYLIB=-l:libraylib.a
LINK_MATH=-lm
POSITION_INDEPENDENT_CODE=-fPIC
MAIN_C=main.c

all: game 

game: main.c gameloader.o gamestate.o libgame.so $(DATA_STRUCTS)
	$(CC) -o $@ $(MAIN_C) gameloader.o gamestate.o $(POSITION_INDEPENDENT_CODE)  $(STATIC_LINK_RAYLIB) $(LINK_MATH)

# Bridge between Raylib and game
gameloader.o: gameloader.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@
gamestate.o: gamestate.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE)  $^ -o $@

# Data structures
vectorentityid.o: vectorentityid.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@

# Main reloadable game base
libgame.o: libgame.c
	touch libgame.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@
libgame.so: libgame.o gamestate.o $(DATA_STRUCTS)
	$(CC) $(SHARED) -o libgame.so libgame.o $(LIBGAME_OBJECTS) $(STATIC_LINK_RAYLIB)
	rm -rfv libgame.so.lockfile

clean:
	rm -rfv game *.o *.so *.lockfile
