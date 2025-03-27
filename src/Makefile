CC = gcc
OBJ = -c
WFLAGS = -Wall
SHARED = -shared
LIBGAME_OBJECTS = gamestate.o hashtable_entityid_spritegroup.o get_txkey_for_tiletype.o specifier.o dungeon_floor.o dungeon_tile.o entity.o em.o sprite.o spritegroup.o race.o libgame.o
STATIC_LINK_RAYLIB = -l:libraylib.a  # Desktop Raylib
POSITION_INDEPENDENT_CODE = -fPIC
MAIN_C = main.c
LINK_MATH = -lm
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS ?= # Allow override

LIBDRAW_OBJECTS = libdraw.o hashtable_entityid_spritegroup.o spritegroup.o sprite.o get_txkey_for_tiletype.o
LIBLOGIC_OBJECTS = liblogic.o gamestate.o hashtable_entityid_spritegroup.o dungeon_floor.o dungeon_tile.o em.o entity.o dungeon.o

# Web build setup
WEB_CC = emcc
WEB_SOURCES = $(MAIN_C) gameloader_web.c libdraw.c liblogic.c gamestate.c hashtable_entityid_spritegroup.c get_txkey_for_tiletype.c specifier.c dungeon_floor.c dungeon_tile.c entity.c em.c sprite.c spritegroup.c race.c dungeon.c
WEB_INCLUDES = -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include
WEB_LINKS = -L. -L /home/darkmage/src/raylib/src -l:libraylib.web.a  # Use web-specific Raylib
EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall
SHELL_FILE = --shell-file /home/darkmage/src/raylib/src/minshell.html
PRELOAD_FILES = --preload-file ./fonts --preload-file ./textures.txt --preload-file ./img
WEB_OPTIONS = -DPLATFORM_WEB -DWEB

# Targets
all: game #index.html

# Desktop build
game: main.o gameloader.o libdraw.so liblogic.so
	$(CC) -o $@ main.o gameloader.o -L. -ldraw -llogic $(RAYLIB_LIBS) $(WFLAGS) $(POSITION_INDEPENDENT_CODE) $(CFLAGS)
	./log_build_stats.sh

# Web build
index.html: $(WEB_SOURCES)
	$(WEB_CC) -o $@ $(WEB_SOURCES) $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS)

# Object files (unchanged below)
main.o: main.c gameloader.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) main.c -o $@

gameloader.o: gameloader.c gameloader.h gamestate.h inputstate.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) gameloader.c -o $@

libdraw.o: libdraw.c libdraw.h
	touch libdraw.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) libdraw.c -o $@

libdraw.so: $(LIBDRAW_OBJECTS)
	$(CC) $(SHARED) -o $@ $(LIBDRAW_OBJECTS) $(RAYLIB_LIBS) $(LINK_MATH) $(WFLAGS) $(CFLAGS)
	rm -rfv libdraw.so.lockfile

liblogic.o: liblogic.c liblogic.h
	touch liblogic.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) liblogic.c -o $@

liblogic.so: $(LIBLOGIC_OBJECTS)
	$(CC) $(SHARED) -o $@ $(LIBLOGIC_OBJECTS) $(LINK_MATH) $(WFLAGS) $(CFLAGS)
	rm -rfv liblogic.so.lockfile

# ... rest of the object file rules unchanged ...

clean:
	rm -rfv game *.o *.so *.lockfile time.txt index.html index.js index.wasm index.data export_to_code
