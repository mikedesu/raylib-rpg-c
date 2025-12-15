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
libdraw_create_spritegroup.cpp \
libdraw_dungeon_floor.cpp \
libdraw.cpp \
liblogic.cpp \
gamestate.cpp \
dungeon_floor.cpp \
dungeon_tile.cpp \

WEB_INCLUDES = -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include
WEB_LINKS = -L. -L /home/darkmage/src/raylib/src -l:libraylib.web.a  # Use web-specific Raylib

EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -s ALLOW_MEMORY_GROWTH

SHELL_FILE = --shell-file minshell.html
PRELOAD_FILES = --preload-file ./fonts --preload-file ./textures.txt --preload-file ./img  --preload-file ./audio/music --preload-file ./music.txt --preload-file ./audio/sfx --preload-file ./sfx.txt --preload-file ./shaders
WEB_OPTIONS = -DPLATFORM_WEB -DWEB -DSPAWN_MONSTERS -DNPCS_ALL_AT_ONCE -DSTART_MESSAGES

# Targets
#all: game test #index.html
all: game 

# Desktop build

OBJ_FILES=main.o \
gameloader.o \
gamestate.o \
libdraw_dungeon_floor.o \
libdraw_create_spritegroup.o \
libdraw.o \
dungeon_floor.o \
dungeon_tile.o \
dungeon_tile.o \
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


gamestate.o: gamestate.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

###################

dungeon_tile.o: dungeon_tile.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

dungeon_floor.o: dungeon_floor.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_dungeon_floor.o: libdraw_dungeon_floor.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw_create_spritegroup.o: libdraw_create_spritegroup.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $^ -c 

libdraw.o: libdraw.cpp 
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


