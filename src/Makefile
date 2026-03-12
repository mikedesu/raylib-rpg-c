

CXX = clang++


OBJ = -c


WFLAGS = -Wall -Werror -std=c++17


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



# Phony targets
.PHONY: all clean docs docs-clean

# Targets
all: game index.html tests

# Desktop build
game: main.o
	$(CXX) $(WFLAGS) $(CXXFLAGS) $(CFLAGS) $^ -L. $(RAYLIB_LIBS)   -o $@ 
	bash log_build_stats.sh


main.o: main.cpp
	$(CXX) $(WFLAGS) $(CXXFLAGS) $(CFLAGS) main.cpp -c  


# Web build
index.html: main.cpp
	$(WEB_CC) -o $@ main.cpp $(WEB_INCLUDES) $(WEB_LINKS) $(EMCC_OPTIONS) $(SHELL_FILE) $(PRELOAD_FILES) $(WEB_OPTIONS) $(CFLAGS) $(CXXFLAGS)

# Unit tests
tests: tests.cpp
	g++ -o tests tests.cpp -lraylib $(CXXFLAGS) $(CFLAGS)

TEST_HEADERS = unit_test.h \
	test_suites/test_gamestate_lifecycle.h \
	test_suites/test_dungeon_bootstrap.h \
	test_suites/test_entity_factory.h \
	test_suites/test_entity_placement.h \
	test_suites/test_inventory.h \
	test_suites/test_tile_cache.h \
	test_suites/test_component_table.h \
	test_suites/test_utility_helpers.h

tests.cpp: $(TEST_HEADERS)
	cxxtestgen --error-printer -o tests.cpp $(TEST_HEADERS)

# Documentation
docs:
	doxygen Doxyfile

docs-clean:
	rm -rf docs/api


clean:
	rm -rfv game *.o *.gch time.txt index.html index.js index.wasm index.data test tests.cpp tests
