CC = gcc
OBJ = -c
WFLAGS = -Wall 
#WFLAGS = -Wall -Werror
SHARED = -shared
STATIC_LINK_RAYLIB = -l:libraylib.a  # Desktop Raylib
POSITION_INDEPENDENT_CODE = -fPIC
MAIN_C = main.c
LINK_MATH = -lm
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS ?= # Allow override

GAMESTATE_OBJECTS = gamestate.o gamestate_light_radius.o gamestate_explored_list.o hashtable_entityid_spritegroup.o dungeon_floor.o dungeon_tile.o dungeon.o keybinding.o path_node.o component_table.o component.o

LIBDRAW_OBJECTS = libdraw.o hashtable_entityid_spritegroup.o spritegroup.o sprite.o dungeon.o gamestate.o gamestate_light_radius.o gamestate_explored_list.o dungeon_floor.o dungeon_tile.o component_table.o component.o libdraw_sort_inventory_menu.o
LIBLOGIC_OBJECTS = liblogic.o $(GAMESTATE_OBJECTS)

# Web build setup
WEB_CC = emcc
WEB_SOURCES = $(MAIN_C) gameloader_web.c libdraw.c liblogic.c gamestate.c gamestate_light_radius.c gamestate_explored_list.c hashtable_entityid_spritegroup.c  specifier.c dungeon_floor.c dungeon_tile.c sprite.c spritegroup.c race.c dungeon.c keybinding.c path_node.c component_table.c component.c
WEB_INCLUDES = -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include
WEB_LINKS = -L. -L /home/darkmage/src/raylib/src -l:libraylib.web.a  # Use web-specific Raylib

EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -s ALLOW_MEMORY_GROWTH
#EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall 

SHELL_FILE = --shell-file minshell.html
PRELOAD_FILES = --preload-file ./fonts --preload-file ./textures.txt --preload-file ./img --preload-file keybindings.ini --preload-file ./audio/music --preload-file ./music.txt --preload-file ./monsters.csv
WEB_OPTIONS = -DPLATFORM_WEB -DWEB

UNIT_TEST_OBJS = unit_test.o component.o component_table.o keybinding.o gamestate.o dungeon_tile.o dungeon_floor.o dungeon.o btree_entityid_int.o

# Targets
all: game test #index.html

# Desktop build
game: main.o gameloader.o gamestate.o gamestate_light_radius.o gamestate_explored_list.o keybinding.o libdraw.o liblogic.o component_table.o component.o hashtable_entityid_spritegroup.o spritegroup.o sprite.o dungeon.o dungeon_floor.o dungeon_tile.o path_node.o libdraw_sort_inventory_menu.o
	$(CC) -o $@ main.o gameloader.o gamestate.o gamestate_light_radius.o gamestate_explored_list.o component_table.o component.o keybinding.o libdraw.o liblogic.o hashtable_entityid_spritegroup.o spritegroup.o sprite.o dungeon.o dungeon_floor.o dungeon_tile.o path_node.o libdraw_sort_inventory_menu.o -L. $(RAYLIB_LIBS) $(WFLAGS) $(POSITION_INDEPENDENT_CODE) $(CFLAGS)
	./log_build_stats.sh

test: $(UNIT_TEST_OBJS)
	$(CC) -o $@ $(UNIT_TEST_OBJS) $(WFLAGS) $(CFLAGS)

unit_test.o: unit_test.c unit_test.h 
	    $(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) unit_test.c -o $@

# Web build
index.html: $(WEB_SOURCES)
	$(WEB_CC) -o $@ $(WEB_SOURCES) $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS)

# Object files (unchanged below)
main.o: main.c gameloader.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) main.c -o $@

gameloader.o: gameloader.c gameloader.h gamestate.h inputstate.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) gameloader.c -o $@

libdraw.o: libdraw.c libdraw.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) libdraw.c -o $@

liblogic.o: liblogic.c liblogic.h
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) liblogic.c -o $@

clean:
	rm -rfv game *.o time.txt index.html index.js index.wasm index.data test
