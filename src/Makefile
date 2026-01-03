CXX = g++
OBJ = -c
WFLAGS = -Wall -std=c++23
SHARED = -shared
STATIC_LINK_RAYLIB = -l:libraylib.a  # Desktop Raylib
MAIN_C = main.cpp
LINK_MATH = -lm
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS ?= # Allow override
# Web build setup
WEB_CC = emcc
WEB_SOURCES=main.cpp 
WEB_INCLUDES = -I. -I /usr/local/include -I /home/darkmage/src/emsdk/upstream/emscripten/cache/sysroot/include
WEB_LINKS = -L. -L /home/darkmage/src/raylib/src -l:libraylib.web.a  # Use web-specific Raylib
EMCC_OPTIONS = -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -s ALLOW_MEMORY_GROWTH
SHELL_FILE = --shell-file minshell.html
PRELOAD_FILES = --preload-file ./fonts --preload-file ./textures.txt --preload-file ./img  --preload-file ./audio/music --preload-file ./music.txt --preload-file ./audio/sfx --preload-file ./sfx.txt --preload-file ./shaders
WEB_OPTIONS = -DPLATFORM_WEB -DWEB -DSPAWN_MONSTERS -DNPCS_ALL_AT_ONCE -DSTART_MESSAGES

# Targets
all: game index.html tests

# Desktop build
game: main.cpp 
	$(CXX) $(WFLAGS) $(CXXFLAGS) $(CFLAGS) $^ -L. $(RAYLIB_LIBS)   -o $@ 
	./log_build_stats.sh

# Web build
index.html: main.cpp
	$(WEB_CC) -o $@ main.cpp $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS)

# Unit tests
tests: tests.cpp
	g++ -o tests tests.cpp -lraylib $(CXXFLAGS) $(CFLAGS)

tests.cpp: unit_test.h
	cxxtestgen --error-printer -o tests.cpp unit_test.h 


clean:
	rm -rfv game *.gch time.txt index.html index.js index.wasm index.data test tests.cpp tests


