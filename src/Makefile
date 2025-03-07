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
CFLAGS ?= # Allow override

all: game

game: main.o gameloader.o libdraw.so liblogic.so
	$(CC) -o $@ main.o gameloader.o -L. -ldraw -llogic $(RAYLIB_LIBS) $(WFLAGS) $(POSITION_INDEPENDENT_CODE) $(CFLAGS)

main.o: main.c gameloader.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) main.c -o $@

gameloader.o: gameloader.c gameloader.h gamestate.h inputstate.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) gameloader.c -o $@

libdraw.o: libdraw.c libdraw.h
	touch libdraw.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) libdraw.c -o $@

libdraw.so: libdraw.o
	$(CC) $(SHARED) -o $@ libdraw.o $(RAYLIB_LIBS) $(LINK_MATH) $(WFLAGS) $(CFLAGS)
	rm -rfv libdraw.so.lockfile

liblogic.o: liblogic.c liblogic.h
	touch liblogic.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) liblogic.c -o $@

# Bundle gamestate and its deps into liblogic.so
liblogic.so: liblogic.o gamestate.o hashtable_entityid_spritegroup.o dungeon_floor.o dungeon_tile.o em.o
	$(CC) $(SHARED) -o $@ liblogic.o gamestate.o hashtable_entityid_spritegroup.o dungeon_floor.o dungeon_tile.o em.o $(LINK_MATH) $(WFLAGS) $(CFLAGS)
	rm -rfv liblogic.so.lockfile

gamestate.o: gamestate.c gamestate.h hashtable_entityid_spritegroup.h dungeon_floor.h em.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) gamestate.c -o $@

hashtable_entityid_spritegroup.o: hashtable_entityid_spritegroup.c hashtable_entityid_spritegroup.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) hashtable_entityid_spritegroup.c -o $@

dungeon_floor.o: dungeon_floor.c dungeon_floor.h dungeon_tile.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) dungeon_floor.c -o $@

dungeon_tile.o: dungeon_tile.c dungeon_tile.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) dungeon_tile.c -o $@

em.o: em.c em.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) em.c -o $@

# Legacy libgame stuff (unchanged)
libgame.o: libgame.c
	touch libgame.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) libgame.c -o $@

libgame.so: $(LIBGAME_OBJECTS)
	$(CC) $(SHARED) -o $@ $(LIBGAME_OBJECTS) $(STATIC_LINK_RAYLIB) $(LINK_MATH) $(WFLAGS) $(CFLAGS)
	rm -rfv libgame.so.lockfile

# Supporting objects (unchanged)
sprite.o: sprite.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) sprite.c -o $@

spritegroup.o: spritegroup.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) spritegroup.c -o $@

entity.o: entity.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) entity.c -o $@

get_txkey_for_tiletype.o: get_txkey_for_tiletype.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) get_txkey_for_tiletype.c -o $@

specifier.o: specifier.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) specifier.c -o $@

race.o: race.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) race.c -o $@

export_to_code: export_to_code.c
	$(CC) -o $@ export_to_code.c $(WFLAGS) $(CFLAGS) -lraylib

clean:
	rm -rfv game *.o *.so *.lockfile time.txt index.html index.js index.wasm index.data export_to_code
