CC=gcc
OBJ=-c
WFLAGS=-Wall -Werror
SHARED=-shared
DATA_STRUCTS=vectorentityid.o 
SCENES=companyscene.o 
LIBGAME_OBJECTS=$(DATA_STRUCTS) $(SCENES) libgame.o  utils.o sprite.o  spritegroup.o entity.o hashtable_entityid_entity.o gamestate.o hashtable_entityid_spritegroup.o dungeonfloor.o get_txkey_for_tiletype.o
STATIC_LINK_RAYLIB=-l:libraylib.a
LINK_MATH=-lm
POSITION_INDEPENDENT_CODE=-fPIC
MAIN_C=main.c
WEB_SOURCES=$(MAIN_C) gameloader.c libgame.c hashtable_entityid_entity.c dungeonfloor.c vectorentityid.c get_txkey_for_tiletype.c hashtable_entityid_spritegroup.c gamestate.c companyscene.c sprite.c spritegroup.c entity.c utils.c /home/darkmage/src/raylib/src/libraylib.a 
MAIN_OBJECTS=gameloader.o gamestate.o companyscene.o hashtable_entityid_entity.o hashtable_entityid_spritegroup.o dungeonfloor.o vectorentityid.o get_txkey_for_tiletype.o



all: game index.html


index.html: main.c $(MAIN_SOURCES)  
	#emcc -o $@ $(WEB_SOURCES) -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include  -L. -L /home/darkmage/src/raylib/src  -s USE_GLFW=3 -s USE_WEBGL2=1 --shell-file /home/darkmage/src/raylib/src/minshell.html --preload-file ./img -s TOTAL_STACK=65MB -s INITIAL_MEMORY=256MB -s ASSERTIONS -DPLATFORM_WEB --preload-file ./fonts --preload-file textures.txt
	emcc -o $@ $(WEB_SOURCES) -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include  -L. -L /home/darkmage/src/raylib/src  -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall --shell-file shell.html --preload-file ./img --preload-file ./fonts --preload-file textures.txt -DPLATFORM_WEB
	#emcc -o $@ $(WEB_SOURCES) -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include  -L. -L /home/darkmage/src/raylib/src  -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall --shell-file shell.html --preload-file ./img -s TOTAL_STACK=65MB -s INITIAL_MEMORY=256MB -DPLATFORM_WEB --preload-file ./fonts --preload-file textures.txt
	#emcc -o $@ $(WEB_SOURCES) -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include  -L. -L /home/darkmage/src/raylib/src  -s USE_GLFW=3 -s USE_WEBGL2=1 --shell-file shell.html --preload-file ./img -s TOTAL_STACK=65MB -s INITIAL_MEMORY=256MB -DPLATFORM_WEB --preload-file ./fonts --preload-file textures.txt
	#emcc -o $@ $(WEB_SOURCES) -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include  -L. -L /home/darkmage/src/raylib/src  -s USE_GLFW=3 -s USE_WEBGL2=1 -s ASYNCIFY --shell-file shell.html --preload-file ./img -s TOTAL_STACK=65MB -s INITIAL_MEMORY=256MB -s ASSERTIONS -DPLATFORM_WEB --preload-file ./fonts --preload-file textures.txt


game: main.c gameloader.o $(LIBGAME_OBJECTS) libgame.so  
	$(CC) -o $@ $(MAIN_C) $(MAIN_OBJECTS) $(POSITION_INDEPENDENT_CODE) $(WFLAGS)

# Bridge between Raylib and game
gameloader.o: gameloader.c # desktop version with hot reloading
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

gamestate.o: gamestate.c 
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

# Rendering Objects

sprite.o: sprite.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

spritegroup.o: spritegroup.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

# Scenes
companyscene.o: companyscene.c 
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

# Data structures and extra functionality/helpers/etc
vectorentityid.o: vectorentityid.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

utils.o: utils.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

dungeonfloor.o: dungeonfloor.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

entity.o: entity.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

hashtable_entityid_entity.o: hashtable_entityid_entity.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

hashtable_entityid_spritegroup.o: hashtable_entityid_spritegroup.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@

get_txkey_for_tiletype.o: get_txkey_for_tiletype.c
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@


# Main reloadable game base
libgame.o: libgame.c
	touch libgame.so.lockfile
	$(CC) $(OBJ) $(POSITION_INDEPENDENT_CODE) $(WFLAGS) $^ -o $@ # hot-reload desktop version
	#$(CC) $(OBJ) $(WFLAGS) $^ $(LIBGAME_OBJECTS) -lraylib -o $@ # web version
libgame.so: $(LIBGAME_OBJECTS)
	$(CC) $(SHARED) -o libgame.so $(LIBGAME_OBJECTS) $(STATIC_LINK_RAYLIB) $(WFLAGS) 
	rm -rfv libgame.so.lockfile

clean:
	rm -rfv game *.o *.so *.lockfile time.txt index.html index.js index.wasm index.data 
