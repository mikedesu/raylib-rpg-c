CC=gcc
OBJ=-c
#WFLAGS=
#-Wall 
WFLAGS=-Wall -Werror
SHARED=-shared
#DATA_STRUCTS=vectorentityid.o 
SCENES=companyscene.o 
LIBGAME_OBJECTS=$(DATA_STRUCTS) $(SCENES) libgame.o  utils.o sprite.o  spritegroup.o gamestate.o hashtable_entityid_spritegroup.o get_txkey_for_tiletype.o dice.o libgame_lua.o specifier.o 
#LIBGAME_OBJECTS=$(DATA_STRUCTS) $(SCENES) libgame.o  utils.o sprite.o  spritegroup.o gamestate.o hashtable_entityid_spritegroup.o dungeonfloor.o get_txkey_for_tiletype.o dice.o libgame_lua.o specifier.o 
#LIBGAME_OBJECTS=$(DATA_STRUCTS) $(SCENES) libgame.o  utils.o sprite.o  spritegroup.o entity.o hashtable_entityid_entity.o gamestate.o hashtable_entityid_spritegroup.o dungeonfloor.o get_txkey_for_tiletype.o dice.o libgame_lua.o
STATIC_LINK_RAYLIB=-l:libraylib.a
#LINK_MATH=-lm
POSITION_INDEPENDENT_CODE=-fPIC
MAIN_C=main.c
WEB_SOURCES=$(MAIN_C) gameloader_web.c libgame.c libgame_lua.c get_txkey_for_tiletype.c hashtable_entityid_spritegroup.c gamestate.c companyscene.c sprite.c spritegroup.c utils.c dice.c specifier.o /home/darkmage/src/raylib/src/libraylib.a /home/darkmage/src/lua/liblua.a
#WEB_SOURCES=$(MAIN_C) gameloader_web.c libgame.c libgame_lua.c dungeonfloor.c get_txkey_for_tiletype.c hashtable_entityid_spritegroup.c gamestate.c companyscene.c sprite.c spritegroup.c utils.c dice.c specifier.o /home/darkmage/src/raylib/src/libraylib.a /home/darkmage/src/lua/liblua.a
#WEB_SOURCES=$(MAIN_C) gameloader_web.c libgame.c libgame_lua.c dungeonfloor.c vectorentityid.c get_txkey_for_tiletype.c hashtable_entityid_spritegroup.c gamestate.c companyscene.c sprite.c spritegroup.c utils.c dice.c specifier.o /home/darkmage/src/raylib/src/libraylib.a /home/darkmage/src/lua/liblua.a
MAIN_OBJECTS=gameloader.o gamestate.o companyscene.o hashtable_entityid_spritegroup.o get_txkey_for_tiletype.o dice.o specifier.o
#MAIN_OBJECTS=gameloader.o gamestate.o companyscene.o hashtable_entityid_spritegroup.o dungeonfloor.o get_txkey_for_tiletype.o dice.o specifier.o
#MAIN_OBJECTS=gameloader.o gamestate.o companyscene.o hashtable_entityid_spritegroup.o dungeonfloor.o vectorentityid.o get_txkey_for_tiletype.o dice.o specifier.o
#MAIN_OBJECTS=gameloader.o gamestate.o companyscene.o hashtable_entityid_entity.o hashtable_entityid_spritegroup.o dungeonfloor.o vectorentityid.o get_txkey_for_tiletype.o dice.o 


WEB_CC=emcc
WEB_INCLUDES=-I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include -I /home/darkmage/src/lua
WEB_LINKS=-L. -L /home/darkmage/src/raylib/src
EMCC_OPTIONS=-s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall
#SHELL_FILE=--shell-file shell.html
SHELL_FILE=--shell-file /home/darkmage/src/raylib/src/minshell.html
#PRELOAD_FILES=--preload-file ./img --preload-file ./fonts --preload-file ./textures.txt --preload-file ./init.lua --preload-file ./test.lua
#PRELOAD_FILES=--preload-file ./fonts --preload-file ./textures.txt --preload-file ./init.lua --preload-file ./test.lua --preload-file ./evildojo.mp3
PRELOAD_FILES=--preload-file ./fonts --preload-file ./textures.txt --preload-file ./init.lua --preload-file ./test.lua 
WEB_OPTIONS=-DPLATFORM_WEB -DWEB

all: game export_to_code


index.html: main.c $(MAIN_SOURCES)  
	#$(WEB_CC) -o $@ $(WEB_SOURCES) $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS)
	$(WEB_CC) -o $@ $(WEB_SOURCES) $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS)


game: main.c gameloader.o $(LIBGAME_OBJECTS) libgame.so  
	$(CC) -o $@ $(MAIN_C) $(MAIN_OBJECTS) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)

# Bridge between Raylib and game
gameloader.o: gameloader.c 
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) $^ -o $@

gamestate.o: gamestate.c 
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS) $^ -o $@

# Rendering Objects

sprite.o: sprite.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS)  $(CFLAGS) $^ -o $@

spritegroup.o: spritegroup.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS)  $(CFLAGS) $^ -o $@

# Scenes
companyscene.o: companyscene.c 
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

# Data structures and extra functionality/helpers/etc
#vectorentityid.o: vectorentityid.c
#	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

utils.o: utils.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

#dungeonfloor.o: dungeonfloor.c
#	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

#entity.o: entity.c
#	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

#hashtable_entityid_entity.o: hashtable_entityid_entity.c
#	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

hashtable_entityid_spritegroup.o: hashtable_entityid_spritegroup.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

get_txkey_for_tiletype.o: get_txkey_for_tiletype.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

dice.o: dice.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

specifier.o: specifier.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

libgame_lua.o: libgame_lua.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@

# Main reloadable game base
libgame.o: libgame.c
	touch libgame.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $(CFLAGS)  $^ -o $@ 
libgame.so: $(LIBGAME_OBJECTS)
	$(CC) $(SHARED) -o libgame.so $(LIBGAME_OBJECTS) $(STATIC_LINK_RAYLIB) -llua5.4 $(WFLAGS)  $(CFLAGS) 
	rm -rfv libgame.so.lockfile

# Tool to help build out the asset packing
export_to_code: export_to_code.c
	$(CC) -o $@ $^ $(WFLAGS) -lraylib



clean:
	rm -rfv game *.o *.so *.lockfile time.txt index.html index.js index.wasm index.data export_to_code
