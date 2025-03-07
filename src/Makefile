CC = gcc
OBJ = -c
WFLAGS = -Wall
SHARED = -shared
LIBGAME_OBJECTS = gamestate.o hashtable_entityid_spritegroup.o get_txkey_for_tiletype.o specifier.o dungeon_floor.o dungeon_tile.o entity.o em.o sprite.o spritegroup.o race.o libgame.o
STATIC_LINK_RAYLIB = -l:libraylib.a
POSITION_INDEPENDENT_CODE = -fPIC
MAIN_C = main.c
LINK_MATH = -lm
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: game

# Main executable
game: main.o gameloader.o libdraw.so
	$(CC) -o $@ main.o gameloader.o -L. -ldraw $(RAYLIB_LIBS) $(WFLAGS) $(POSITION_INDEPENDENT_CODE)

# Core objects
main.o: main.c gameloader.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) main.c -o $@

gameloader.o: gameloader.c gameloader.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) gameloader.c -o $@

# New reloadable drawing base
libdraw.o: libdraw.c libdraw.h
	touch libdraw.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) libdraw.c -o $@

libdraw.so: libdraw.o
	$(CC) $(SHARED) -o $@ libdraw.o $(RAYLIB_LIBS) $(LINK_MATH) $(WFLAGS)
	rm -rfv libdraw.so.lockfile

# Legacy libgame stuff (keep for now, but not used)
libgame.o: libgame.c
	touch libgame.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) libgame.c -o $@

libgame.so: $(LIBGAME_OBJECTS)
	$(CC) $(SHARED) -o $@ $(LIBGAME_OBJECTS) $(STATIC_LINK_RAYLIB) $(LINK_MATH) $(WFLAGS)
	rm -rfv libgame.so.lockfile

# Supporting objects (unchanged)
gamestate.o: gamestate.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) gamestate.c -o $@

sprite.o: sprite.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) sprite.c -o $@

spritegroup.o: spritegroup.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) spritegroup.c -o $@

dungeon_floor.o: dungeon_floor.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) dungeon_floor.c -o $@

entity.o: entity.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) entity.c -o $@

em.o: em.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) em.c -o $@

dungeon_tile.o: dungeon_tile.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) dungeon_tile.c -o $@

hashtable_entityid_spritegroup.o: hashtable_entityid_spritegroup.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) hashtable_entityid_spritegroup.c -o $@

get_txkey_for_tiletype.o: get_txkey_for_tiletype.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) get_txkey_for_tiletype.c -o $@

specifier.o: specifier.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) specifier.c -o $@

race.o: race.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) race.c -o $@

# Tool (unchanged)
export_to_code: export_to_code.c
	$(CC) -o $@ export_to_code.c $(WFLAGS) -lraylib

clean:
	rm -rfv game *.o *.so *.lockfile time.txt index.html index.js index.wasm index.data export_to_code
