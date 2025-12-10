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
try_entity_attack.cpp \
try_entity_open_door.cpp \
try_entity_stairs.cpp \
manage_inventory.cpp \
update_player_tiles_explored.cpp \
draw_inventory_menu.cpp \
libdraw_message_history.cpp \
libdraw_version.cpp \
libdraw_title_screen.cpp \
libdraw_help_menu.cpp \
libdraw_draw_character_creation_screen.cpp \
libdraw_load_textures.cpp \
libdraw_unload_textures.cpp \
libdraw_create_spritegroup.cpp \
libdraw_create_sg_byid.cpp \
libdraw_hud.cpp \
libdraw_check_default_animations.cpp \
libdraw_frame_2d.cpp \
libdraw_from_texture.cpp \
libdraw_to_texture.cpp \
libdraw_handle_gamestate_flag.cpp \
libdraw_handle_dirty_entities.cpp \
libdraw_update_sprite.cpp \
libdraw_update_sprites.cpp \
libdraw_dungeon_floor.cpp \
libdraw_message_box.cpp \
libdraw_sprite.cpp \
libdraw_shaders.cpp \
libdraw_weapon_sprite.cpp \
libdraw_shield_sprite.cpp \
libdraw_update_weapon_for_entity.cpp \
libdraw_update_shield_for_entity.cpp \
libdraw_set_sg_is_attacking.cpp \
libdraw_set_sg.cpp \
libdraw_load_music.cpp \
libdraw_load_sfx.cpp \
libdraw_debug_panel.cpp \
libdraw_update_debug_panel.cpp \
libdraw_handle_debug_panel.cpp \
libdraw_get_weapon_sprite.cpp \
libdraw_get_shield_sprite.cpp \
libdraw_entity_sprite.cpp \
libdraw_player_target_box.cpp \
libdraw_camera_lock_on.cpp \
libdraw.cpp \
liblogic_is_entity_adjacent.cpp \
try_entity_pickup.cpp \
liblogic_update_player_state.cpp \
liblogic_handle_npc.cpp \
liblogic_handle_npcs.cpp \
liblogic_handle_input.cpp \
liblogic_handle_input_inventory.cpp \
liblogic_handle_input_title_scene.cpp \
liblogic_handle_input_main_menu_scene.cpp \
liblogic_handle_input_help_menu.cpp \
liblogic_handle_input_character_creation_scene.cpp \
liblogic_handle_input_gameplay_controlmode_player.cpp \
liblogic_handle_input_gameplay_scene.cpp \
liblogic_update_debug_panel_buffer.cpp \
liblogic.cpp \
gamestate_inventory.cpp \
gamestate.cpp \
dungeon_floor.cpp \
dungeon_tile.cpp \
sprite.cpp \
spritegroup.cpp \

WEB_INCLUDES = -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include
WEB_LINKS = -L. -L /home/darkmage/src/raylib/src -l:libraylib.web.a  # Use web-specific Raylib

EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -s ALLOW_MEMORY_GROWTH

SHELL_FILE = --shell-file minshell.html
PRELOAD_FILES = --preload-file ./fonts --preload-file ./textures.txt --preload-file ./img  --preload-file ./audio/music --preload-file ./music.txt --preload-file ./audio/sfx --preload-file ./sfx.txt --preload-file ./shaders
WEB_OPTIONS = -DPLATFORM_WEB -DWEB -DSPAWN_MONSTERS

# Targets
#all: game test #index.html
all: game 

# Desktop build

OBJ_FILES=main.o \
gameloader.o \
gamestate_inventory.o \
gamestate.o \
try_entity_open_door.o \
try_entity_stairs.o \
try_entity_attack.o \
manage_inventory.o \
update_player_tiles_explored.o \
draw_inventory_menu.o \
libdraw_message_history.o \
libdraw_version.o \
libdraw_title_screen.o \
libdraw_help_menu.o \
libdraw_draw_character_creation_screen.o \
libdraw_load_textures.o \
libdraw_unload_textures.o \
libdraw_hud.o \
libdraw_check_default_animations.o \
libdraw_frame_2d.o \
libdraw_from_texture.o \
libdraw_to_texture.o \
libdraw_create_sg_byid.o \
libdraw_handle_gamestate_flag.o \
libdraw_handle_dirty_entities.o \
libdraw_update_sprite.o \
libdraw_update_sprites.o \
libdraw_dungeon_floor.o \
libdraw_message_box.o \
libdraw_shaders.o \
libdraw_sprite.o \
libdraw_weapon_sprite.o \
libdraw_shield_sprite.o \
libdraw_update_weapon_for_entity.o \
libdraw_update_shield_for_entity.o \
libdraw_set_sg_is_attacking.o \
libdraw_set_sg.o \
libdraw_load_music.o \
libdraw_load_sfx.o \
libdraw_debug_panel.o \
libdraw_update_debug_panel.o \
libdraw_handle_debug_panel.o \
libdraw_get_weapon_sprite.o \
libdraw_get_shield_sprite.o \
libdraw_entity_sprite.o \
libdraw_player_target_box.o \
libdraw_camera_lock_on.o \
libdraw_create_spritegroup.o \
libdraw_update_debug_panel.o \
libdraw.o \
spritegroup.o \
sprite.o \
dungeon_floor.o \
dungeon_tile.o \
dungeon_tile.o \
liblogic_is_entity_adjacent.o \
try_entity_pickup.o \
liblogic_update_player_state.o \
liblogic_handle_npc.o \
liblogic_handle_npcs.o \
liblogic_handle_input.o \
liblogic_handle_input_inventory.o \
liblogic_handle_input_title_scene.o \
liblogic_handle_input_gameplay_scene.o \
liblogic_handle_input_main_menu_scene.o \
liblogic_handle_input_help_menu.o \
liblogic_handle_input_character_creation_scene.o \
liblogic_handle_input_gameplay_controlmode_player.o \
liblogic_update_debug_panel_buffer.o \
liblogic.o \



game: $(OBJ_FILES)
	./generate_sfx_header.sh
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

try_entity_open_door.o: try_entity_open_door.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

sprite.o: sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

spritegroup.o: spritegroup.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

dungeon_tile.o: dungeon_tile.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

dungeon_floor.o: dungeon_floor.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 



###################

libdraw_hud.o: libdraw_hud.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_check_default_animations.o: libdraw_check_default_animations.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_to_texture.o: libdraw_to_texture.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_frame_2d.o: libdraw_frame_2d.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_from_texture.o: libdraw_from_texture.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_create_sg_byid.o: libdraw_create_sg_byid.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_handle_dirty_entities.o: libdraw_handle_dirty_entities.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_handle_gamestate_flag.o: libdraw_handle_gamestate_flag.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_update_sprite.o: libdraw_update_sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_update_sprites.o: libdraw_update_sprites.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_dungeon_floor.o: libdraw_dungeon_floor.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_message_box.o: libdraw_message_box.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_message_history.o: libdraw_message_history.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_version.o: libdraw_version.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_title_screen.o: libdraw_title_screen.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_help_menu.o: libdraw_help_menu.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

draw_inventory_menu.o: draw_inventory_menu.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_draw_character_creation_screen.o: libdraw_draw_character_creation_screen.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_load_textures.o: libdraw_load_textures.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_unload_textures.o: libdraw_unload_textures.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


libdraw_shaders.o: libdraw_shaders.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_set_sg.o: libdraw_set_sg.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_sprite.o: libdraw_sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_weapon_sprite.o: libdraw_weapon_sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 
libdraw_shield_sprite.o: libdraw_shield_sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_update_weapon_for_entity.o: libdraw_update_weapon_for_entity.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_update_shield_for_entity.o: libdraw_update_shield_for_entity.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_set_sg_is_attacking.o: libdraw_set_sg_is_attacking.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_entity_sprite.o: libdraw_entity_sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_get_weapon_sprite.o: libdraw_get_weapon_sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_get_shield_sprite.o: libdraw_get_shield_sprite.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_handle_debug_panel.o: libdraw_handle_debug_panel.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_update_debug_panel.o: libdraw_update_debug_panel.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_debug_panel.o: libdraw_debug_panel.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


libdraw_load_music.o: libdraw_load_music.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_load_sfx.o: libdraw_load_sfx.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_player_target_box.o: libdraw_player_target_box.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_camera_lock_on.o: libdraw_camera_lock_on.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_create_spritegroup.o: libdraw_create_spritegroup.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw.o: libdraw.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


try_entity_stairs.o: try_entity_stairs.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


manage_inventory.o: manage_inventory.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

update_player_tiles_explored.o: update_player_tiles_explored.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_is_entity_adjacent.o: liblogic_is_entity_adjacent.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


try_entity_attack.o: try_entity_attack.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

try_entity_pickup.o: try_entity_pickup.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


liblogic_update_player_state.o: liblogic_update_player_state.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 


liblogic_handle_npc.o: liblogic_handle_npc.cpp 	
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_npcs.o: liblogic_handle_npcs.cpp
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



liblogic_update_debug_panel_buffer.o: liblogic_update_debug_panel_buffer.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

liblogic_handle_input.o: liblogic_handle_input.cpp
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
	rm -rfv game *.o *.gch time.txt index.html index.js index.wasm index.data test


