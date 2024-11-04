make clean;

{ time make } 2>time.txt;

mv game game_gcc;

BUILD_TIME=$(awk '{print $8}' time.txt);

#echo $BUILD_TIME;
#echo $CLANG_BUILD_TIME;
TIMESTAMP=$(date +%s);
LOC=$(cat *.h *.c | egrep -v "^[[:space:]]*//" | egrep -v "^$" | wc -l);
LUA_LOC=$(cat *.lua | egrep -v "^[[:space:]]*--" | egrep -v "^$" | wc -l);
GCC_SIZE=$(ls -al game_gcc | awk '{print $5}');
GCC_LIBSIZE=$(ls -al libgame.so | awk '{print $5}');


make clean;	
#echo $TIMESTAMP,$SIZE,$LIBSIZE,$LOC,$BUILD_TIME;
#echo $TIMESTAMP,$GCC_SIZE,$CLANG_SIZE,$LIBSIZE,$LOC,$LUA_LOC,$BUILD_TIME >> build-stats.csv

{ time make CC=clang } 2>time_clang.txt;
mv game game_clang;
CLANG_BUILD_TIME=$(awk '{print $9}' time_clang.txt);
CLANG_SIZE=$(ls -al game_clang | awk '{print $5}');
CLANG_LIBSIZE=$(ls -al libgame.so | awk '{print $5}');


{ time make index.html } 2>time_emcc.txt;

EMCC_BUILD_TIME=$(awk '{print $9}' time_emcc.txt);
EMCC_DATA_SIZE=$(ls -al index.data | awk '{print $5}');
EMCC_HTML_SIZE=$(ls -al index.html | awk '{print $5}');
EMCC_JS_SIZE=$(ls -al index.js | awk '{print $5}');
EMCC_WASM_SIZE=$(ls -al index.wasm | awk '{print $5}');

make clean;

#echo "timestamp,gcc_size,clang_size,gcc_libsize,clang_libsize,loc,lua_loc,gcc_build_time,clang_build_time" > build-stats.csv
echo $TIMESTAMP,$GCC_SIZE,$CLANG_SIZE,$GCC_LIBSIZE,$CLANG_LIBSIZE,$EMCC_DATA_SIZE,$EMCC_HTML_SIZE,$EMCC_JS_SIZE,$EMCC_WASM_SIZE,$LOC,$LUA_LOC,$BUILD_TIME,$CLANG_BUILD_TIME,$EMCC_BUILD_TIME >> build-stats.csv

#python3 graph_loc.py build-stats.csv loc 2>/dev/null;
#python3 graph_loc.py build-stats.csv lua_loc 2>/dev/null;
#python3 graph_loc.py build-stats.csv loc lua_loc 2>/dev/null;
#python3 graph_loc.py build-stats.csv gcc_build_time clang_build_time 2>/dev/null;
#python3 graph_loc.py build-stats.csv gcc_size clang_size 2>/dev/null;
#python3 graph_loc.py build-stats.csv gcc_libsize clang_libsize 2>/dev/null;
#python3 graph_loc.py build-stats.csv emcc_build_time 2>/dev/null;
#python3 graph_loc.py build-stats.csv emcc_data_size 2>/dev/null;
#python3 graph_loc.py build-stats.csv emcc_html_size 2>/dev/null;
#python3 graph_loc.py build-stats.csv emcc_js_size 2>/dev/null;
#python3 graph_loc.py build-stats.csv emcc_wasm_size 2>/dev/null;

rm -rf time.txt time_clang.txt time_emcc.txt;
