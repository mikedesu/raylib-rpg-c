CC=gcc
OBJ=-c
SHARED=-shared
DATA_STRUCTS=vectorentityid.o 
SCENES=companyscene.o 
LIBGAME_OBJECTS=$(DATA_STRUCTS) $(SCENES) libgame.o gamestate.o utils.o sprite.o setdebugpanel.o
STATIC_LINK_RAYLIB=-l:libraylib.a
LINK_MATH=-lm
POSITION_INDEPENDENT_CODE=-fPIC
MAIN_C=main.c

all: game  

game: main.c gameloader.o $(LIBGAME_OBJECTS) libgame.so companyscene.o 
	$(CC) -o $@ $(MAIN_C) gameloader.o gamestate.o companyscene.o $(POSITION_INDEPENDENT_CODE) 
	#$(CC) -o $@ $(MAIN_C) gameloader.o $(LIBGAME_OBJECTS) $(POSITION_INDEPENDENT_CODE)  $(STATIC_LINK_RAYLIB) $(LINK_MATH)

# Bridge between Raylib and game
gameloader.o: gameloader.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@

gamestate.o: gamestate.c 
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@
	#$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(STATIC_LINK_RAYLIB) $^ -o $@

# Rendering Objects

sprite.o: sprite.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@



# Scenes

companyscene.o: companyscene.c 
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@
	#$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(STATIC_LINK_RAYLIB) $^ -o $@

# Data structures and extra functionality/helpers/etc
vectorentityid.o: vectorentityid.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@

utils.o: utils.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@



setdebugpanel.o: setdebugpanel.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@



# Main reloadable game base
libgame.o: libgame.c
	touch libgame.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $^ -o $@
libgame.so: $(LIBGAME_OBJECTS)
	#$(CC) $(SHARED) -o libgame.so $(LIBGAME_OBJECTS) 
	$(CC) $(SHARED) -o libgame.so $(LIBGAME_OBJECTS) $(STATIC_LINK_RAYLIB)
	rm -rfv libgame.so.lockfile

clean:
	rm -rfv game *.o *.so *.lockfile time.txt
