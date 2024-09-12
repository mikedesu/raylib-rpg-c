make clean;

{ time make } 2>time.txt;

BUILD_TIME=$(awk '{print $8}' time.txt);

TIMESTAMP=$(date +%s);

LOC=$(cat *.h *.c | egrep -v "^[[:space:]]*//" | egrep -v "^$" | wc -l);

SIZE=$(ls -al game | awk '{print $5}');

LIBSIZE=$(ls -al libgame.so | awk '{print $5}');

echo $TIMESTAMP,$SIZE,$LIBSIZE,$LOC,$BUILD_TIME >> build-stats.csv

make clean;
