CXX = g++
OBJ = -c
WFLAGS = -Wall -std=c++17
SHARED = -shared
STATIC_LINK_RAYLIB = -l:libraylib.a  # Desktop Raylib
MAIN_C = main.cpp
LINK_MATH = -lm
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS ?= # Allow override

GAMESTATE_OBJECTS = gamestate.o hashtable_entityid_spritegroup.o dungeon_floor.o dungeon_tile.o dungeon.o keybinding.o path_node.o component_table.o component.o

LIBDRAW_OBJECTS = libdraw.o hashtable_entityid_spritegroup.o spritegroup.o sprite.o dungeon.o gamestate.o dungeon_floor.o dungeon_tile.o component_table.o component.o libdraw_sort_inventory_menu.o
LIBLOGIC_OBJECTS = liblogic.o $(GAMESTATE_OBJECTS)

# Web build setup
WEB_CC = emcc
WEB_SOURCES = $(MAIN_C) gameloader_web.cpp libdraw.cpp libdraw_sort_inventory_menu.cpp liblogic.cpp gamestate.cpp hashtable_entityid_spritegroup.cpp  specifier.cpp dungeon_floor.cpp dungeon_tile.cpp sprite.cpp spritegroup.cpp race.cpp dungeon.cpp keybinding.cpp path_node.cpp component_table.cpp component.cpp
WEB_INCLUDES = -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include
WEB_LINKS = -L. -L /home/darkmage/src/raylib/src -l:libraylib.web.a  # Use web-specific Raylib

EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -s ALLOW_MEMORY_GROWTH
#EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall 

SHELL_FILE = --shell-file minshell.html
PRELOAD_FILES = --preload-file ./fonts --preload-file ./textures.txt --preload-file ./img --preload-file keybindings.ini --preload-file ./audio/music --preload-file ./music.txt --preload-file ./monsters.csv
WEB_OPTIONS = -DPLATFORM_WEB -DWEB

#UNIT_TEST_OBJS = unit_test.o component.o component_table.o keybinding.o gamestate.o dungeon_tile.o dungeon_floor.o dungeon.o btree_entityid_int.o

# Targets
#all: game test #index.html
all: game 

# Desktop build

#game: main.o gameloader.o gamestate.o keybinding.o libdraw.o liblogic.o component_table.o component.o hashtable_entityid_spritegroup.o spritegroup.o sprite.o dungeon.o dungeon_floor.o dungeon_tile.o path_node.o libdraw_sort_inventory_menu.o
game: main.o gameloader.o gamestate.o keybinding.o libdraw.o hashtable_entityid_spritegroup.o spritegroup.o sprite.o dungeon.o dungeon_floor.o dungeon_tile.o dungeon_tile.o path_node.o liblogic.o
	$(CXX) -o $@ $^ -L. $(RAYLIB_LIBS) $(WFLAGS) $(CFLAGS)
	#./log_build_stats.sh

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

gameloader.o: gameloader.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

gamestate.o: gamestate.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

keybinding.o: keybinding.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

sprite.o: sprite.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

spritegroup.o: spritegroup.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

hashtable_entityid_spritegroup.o: hashtable_entityid_spritegroup.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

dungeon_tile.o: dungeon_tile.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

dungeon_floor.o: dungeon_floor.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

dungeon.o: dungeon.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

libdraw.o: libdraw.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

path_node.o: path_node.cpp
	$(CXX) $(CXXFLAGS) $^ -c 

liblogic.o: liblogic.cpp
	$(CXX) $(CXXFLAGS) $^ -c








#test: $(UNIT_TEST_OBJS)
#$(CC) -o $@ $(UNIT_TEST_OBJS) $(WFLAGS) $(CFLAGS)

#unit_test.o: unit_test.cpp unit_test.h 
#	    $(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) unit_test.cpp -o $@

# Web build
index.html: $(WEB_SOURCES)
	$(WEB_CC) -o $@ $(WEB_SOURCES) $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS)

# Object files (unchanged below)
# C++ source files
#%.o: %.cpp
#	$(CXX) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CXXFLAGS) $< -o $@

clean:
	rm -rfv game *.o time.txt index.html index.js index.wasm index.data test
