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
			libdraw_load_textures.cpp \
			libdraw_unload_textures.cpp \
			libdraw_dungeon_floor_tile.cpp \
			libdraw_create_spritegroup.cpp \
			libdraw_update_debug_panel.cpp \
			libdraw_hud.cpp \
		  libdraw_camera_lock_on.cpp \
			libdraw_dungeon_tiles_2d.cpp \
			libdraw.cpp \
			liblogic_drop_item.cpp \
			liblogic_try_entity_attack.cpp \
			liblogic_try_entity_pickup.cpp \
			liblogic_add_message.cpp \
			liblogic_update_player_state.o \
			liblogic_tile_npc_living_count.cpp \
			liblogic_tile_has_box.cpp \
			liblogic_try_entity_move.cpp \
			liblogic_create_weapon.cpp \
			liblogic_create_player.cpp \
			liblogic_create_npc.cpp \
			liblogic_create_npc_set_stats.cpp \
			liblogic_handle_npc.cpp \
			liblogic_handle_input.cpp \
			liblogic_handle_camera_move.cpp \
			liblogic_handle_input_inventory.cpp \
			liblogic_handle_input_title_scene.cpp \
			liblogic_handle_input_main_menu_scene.cpp \
			liblogic_handle_input_help_menu.cpp \
			liblogic_handle_input_character_creation_scene.cpp \
			liblogic_handle_input_gameplay_controlmode_player.cpp \
			liblogic_handle_input_gameplay_scene.cpp \
			liblogic_init_dungeon.cpp \
			liblogic_cycle_messages.cpp \
			liblogic_update_debug_panel_buffer.cpp \
			liblogic.cpp \
			gamestate_inventory.cpp \
			gamestate.cpp \
			dungeon_floor.cpp \
			dungeon_tile.cpp \
			sprite.cpp \
			spritegroup.cpp \
			race.cpp \
			dungeon.cpp \

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
		  gamestate.o \
		  libdraw_draw_inventory_menu.o \
		  libdraw_message_history.o \
		  libdraw_version.o \
		  libdraw_title_screen.o \
		  libdraw_help_menu.o \
		  libdraw_draw_character_creation_screen.o \
		  libdraw_load_textures.o \
		  libdraw_unload_textures.o \
		  libdraw_hud.o \
		  libdraw_camera_lock_on.o \
		  libdraw_dungeon_tiles_2d.o \
		  libdraw_dungeon_floor_tile.o \
		  libdraw_create_spritegroup.o \
		  libdraw_update_debug_panel.o \
		  libdraw.o \
		  spritegroup.o \
		  sprite.o \
		  dungeon.o \
		  dungeon_floor.o \
		  dungeon_tile.o \
		  dungeon_tile.o \
		  liblogic_drop_item.o \
		  liblogic_add_message.o \
		  liblogic_try_entity_attack.o \
		  liblogic_try_entity_pickup.o \
		  liblogic_update_player_state.o \
		  liblogic_tile_npc_living_count.o \
		  liblogic_tile_has_box.o \
		  liblogic_try_entity_move.o \
		  liblogic_create_weapon.o \
		  liblogic_create_player.o \
		  liblogic_create_npc.o \
		  liblogic_create_npc_set_stats.o \
		  liblogic_handle_npc.o \
		  liblogic_handle_input.o \
		  liblogic_handle_camera_move.o \
		  liblogic_handle_input_inventory.o \
		  liblogic_handle_input_title_scene.o \
		  liblogic_handle_input_gameplay_scene.o \
		  liblogic_handle_input_main_menu_scene.o \
		  liblogic_handle_input_help_menu.o \
		  liblogic_handle_input_character_creation_scene.o \
		  liblogic_handle_input_gameplay_controlmode_player.o \
		  liblogic_update_debug_panel_buffer.o \
		  liblogic_init_dungeon.o \
		  liblogic_cycle_messages.o \
		  liblogic.o \



game: $(OBJ_FILES)
	$(CXX) $(WFLAGS) $(CFLAGS) $^ -L. $(RAYLIB_LIBS)   -o $@ 
	./log_build_stats.sh

main.o: main.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

gameloader.o: gameloader.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

###################

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

libdraw_load_textures.o: libdraw_load_textures.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_unload_textures.o: libdraw_unload_textures.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_hud.o: libdraw_hud.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_camera_lock_on.o: libdraw_camera_lock_on.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_dungeon_tiles_2d.o: libdraw_dungeon_tiles_2d.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_dungeon_floor_tile.o: libdraw_dungeon_floor_tile.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_create_spritegroup.o: libdraw_create_spritegroup.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_update_debug_panel.o: libdraw_update_debug_panel.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw.o: libdraw.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

###################

###################

liblogic_drop_item.o: liblogic_drop_item.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_try_entity_attack.o: liblogic_try_entity_attack.cpp
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

liblogic_create_weapon.o: liblogic_create_weapon.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_create_player.o: liblogic_create_player.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_create_npc.o: liblogic_create_npc.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 
	
liblogic_create_npc_set_stats.o: liblogic_create_npc_set_stats.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_npc.o: liblogic_handle_npc.cpp liblogic_handle_npc.h liblogic_try_entity_move.h gamestate.h
	$(CXX) $(WFLAGS) $(CXXFLAGS) liblogic_handle_npc.cpp -c 

liblogic_handle_camera_move.o: liblogic_handle_camera_move.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input_inventory.o: liblogic_handle_input_inventory.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input_title_scene.o: liblogic_handle_input_title_scene.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input_main_menu_scene.o: liblogic_handle_input_main_menu_scene.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input_help_menu.o: liblogic_handle_input_help_menu.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input_character_creation_scene.o: liblogic_handle_input_character_creation_scene.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input_gameplay_controlmode_player.o: liblogic_handle_input_gameplay_controlmode_player.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input_gameplay_scene.o: liblogic_handle_input_gameplay_scene.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_init_dungeon.o: liblogic_init_dungeon.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_cycle_messages.o: liblogic_cycle_messages.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_update_debug_panel_buffer.o: liblogic_update_debug_panel_buffer.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input.o: liblogic_handle_input.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic.o: liblogic.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c

#ComponentTable.o: ComponentTable.h ComponentTable.cpp ComponentTraits.h
#	$(CXX) $(WFLAGS) $(CXXFLAGS) ComponentTable.cpp -c


#test: $(UNIT_TEST_OBJS)
#$(CC) -o $@ $(UNIT_TEST_OBJS) $(WFLAGS) $(CFLAGS)

#unit_test.o: unit_test.cpp unit_test.h 
#	    $(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) unit_test.cpp -o $@

# Web build
index.html: $(WEB_SOURCES)
	$(WEB_CC) -o $@ $(WEB_SOURCES) $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS)

clean:
	rm -rfv game *.o *.gch time.txt index.html index.js index.wasm index.data test


