#!/usr/bin/zsh

FILENAME="sfx.h";

echo "#pragma once" > $FILENAME;

ct=0; 
for i in `cat sfx.txt`; do 
    a=$(echo SFX_$i $ct | sed 's/.wav//' | tr '[:lower:]' '[:upper:]'); 
    echo "#define" $a; 
    ct=$((ct+1)); 
done >> $FILENAME;


