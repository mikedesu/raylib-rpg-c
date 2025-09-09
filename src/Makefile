CXX = g++
OBJ = -c
WFLAGS = -Wall -std=c++17
SHARED = -shared
STATIC_LINK_RAYLIB = -l:libraylib.a  # Desktop Raylib
MAIN_C = main.cpp
LINK_MATH = -lm
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS ?= # Allow override

# Web build setup
WEB_CC = emcc
WEB_SOURCES=main.cpp \
			gameloader_web.cpp \
			libdraw_draw_inventory_menu.cpp \
			libdraw_message_history.cpp \
			libdraw_version.cpp \
			libdraw_title_screen.cpp \
			libdraw_help_menu.cpp \
			libdraw_draw_character_creation_screen.cpp \
			libdraw.cpp \
		    liblogic_drop_item.cpp \
		    liblogic_get_random_loc.cpp \
			liblogic_try_entity_pickup.cpp \
			liblogic_add_message.cpp \
			liblogic_update_player_state.o \
			liblogic_tile_npc_living_count.cpp \
			liblogic_tile_has_box.cpp \
			liblogic_try_entity_move.cpp \
			liblogic.cpp \
			gamestate_equipped_weapon.cpp \
			gamestate_inventory.cpp \
			gamestate.cpp \
			dungeon_floor.cpp \
			dungeon_tile.cpp \
			sprite.cpp \
			spritegroup.cpp \
			race.cpp \
			dungeon.cpp \
			path_node.cpp \

WEB_INCLUDES = -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include
WEB_LINKS = -L. -L /home/darkmage/src/raylib/src -l:libraylib.web.a  # Use web-specific Raylib

EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -s ALLOW_MEMORY_GROWTH

SHELL_FILE = --shell-file minshell.html
PRELOAD_FILES = --preload-file ./fonts --preload-file ./textures.txt --preload-file ./img  --preload-file ./audio/music --preload-file ./music.txt --preload-file ./monsters.csv
WEB_OPTIONS = -DPLATFORM_WEB -DWEB

# Targets
#all: game test #index.html
all: game 

# Desktop build

OBJ_FILES=main.o \
		  gameloader.o \
		  gamestate_inventory.o \
		  gamestate_equipped_weapon.o \
		  gamestate.o \
		  libdraw_draw_inventory_menu.o \
		  libdraw_message_history.o \
		  libdraw_version.o \
		  libdraw_title_screen.o \
		  libdraw_help_menu.o \
		  libdraw_draw_character_creation_screen.o \
		  libdraw.o \
		  spritegroup.o \
		  sprite.o \
		  dungeon.o \
		  dungeon_floor.o \
		  dungeon_tile.o \
		  dungeon_tile.o \
		  path_node.o \
		  liblogic_drop_item.o \
		  liblogic_get_random_loc.o \
		  liblogic_add_message.o \
		  liblogic_try_entity_pickup.o \
		  liblogic_update_player_state.o \
		  liblogic_tile_npc_living_count.o \
		  liblogic_tile_has_box.o \
		  liblogic_try_entity_move.o \
		  liblogic.o \



game: $(OBJ_FILES)
	$(CXX) $(WFLAGS) $(CFLAGS) $^ -L. $(RAYLIB_LIBS)   -o $@ 
	./log_build_stats.sh

main.o: main.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

gameloader.o: gameloader.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

###################

gamestate_equipped_weapon.o: gamestate_equipped_weapon.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

gamestate_inventory.o: gamestate_inventory.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

gamestate.o: gamestate.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

###################

sprite.o: sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

spritegroup.o: spritegroup.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

dungeon_tile.o: dungeon_tile.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

dungeon_floor.o: dungeon_floor.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

dungeon.o: dungeon.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


###################
libdraw_message_history.o: libdraw_message_history.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_version.o: libdraw_version.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_title_screen.o: libdraw_title_screen.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_help_menu.o: libdraw_help_menu.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_draw_inventory_menu.o: libdraw_draw_inventory_menu.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_draw_character_creation_screen.o: libdraw_draw_character_creation_screen.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw.o: libdraw.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

###################


path_node.o: path_node.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

###################
liblogic_drop_item.o: liblogic_drop_item.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_get_random_loc.o: liblogic_get_random_loc.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_try_entity_pickup.o: liblogic_try_entity_pickup.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_add_message.o: liblogic_add_message.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_update_player_state.o: liblogic_update_player_state.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_tile_npc_living_count.o: liblogic_tile_npc_living_count.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_try_entity_move.o: liblogic_try_entity_move.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_tile_has_box.o: liblogic_tile_has_box.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic.o: liblogic.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c



#test: $(UNIT_TEST_OBJS)
#$(CC) -o $@ $(UNIT_TEST_OBJS) $(WFLAGS) $(CFLAGS)

#unit_test.o: unit_test.cpp unit_test.h 
#	    $(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) unit_test.cpp -o $@

# Web build
index.html: $(WEB_SOURCES)
	$(WEB_CC) -o $@ $(WEB_SOURCES) $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS)

clean:
	rm -rfv game *.o time.txt index.html index.js index.wasm index.data test


