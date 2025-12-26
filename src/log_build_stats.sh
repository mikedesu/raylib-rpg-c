#!/usr/bin/bash

TIMESTAMP=$(date +%s)


COST=$(scc -i cpp,h,py,sh,frag | grep -i cost | awk '{print $6}');
LINES=$(scc -i cpp,h,py,sh,frag | grep -i total | awk '{print $6}')
MONTHS=$(scc -i cpp,h,py,sh,frag | grep -i effort | awk '{print $5}');

#echo timestamp,lines,months,cost

#echo $LINES
#echo $TIMESTAMP 
#echo $MONTHS
#echo $COST 
echo $TIMESTAMP $LINES $MONTHS $COST >> build-stats.csv
