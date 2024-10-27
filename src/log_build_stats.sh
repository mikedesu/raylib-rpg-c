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

make clean;

#echo "timestamp,gcc_size,clang_size,gcc_libsize,clang_libsize,loc,lua_loc,gcc_build_time,clang_build_time" > build-stats.csv
echo $TIMESTAMP,$GCC_SIZE,$CLANG_SIZE,$GCC_LIBSIZE,$CLANG_LIBSIZE,$LOC,$LUA_LOC,$BUILD_TIME,$CLANG_BUILD_TIME >> build-stats.csv

python3 graph_loc.py build-stats.csv loc ;
python3 graph_loc.py build-stats.csv lua_loc ;
python3 graph_loc.py build-stats.csv gcc_build_time clang_build_time;
python3 graph_loc.py build-stats.csv gcc_size clang_size;
python3 graph_loc.py build-stats.csv gcc_libsize clang_libsize;

rm time.txt time_clang.txt game_gcc game_clang;
