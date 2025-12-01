#!/usr/bin/zsh


# lowercase all files in audio/sfx/*.wav
for file in audio/sfx/*.wav; do
    if [ -f "$file" ]; then
        lowercase_file=$(echo "$file" | tr '[:upper:]' '[:lower:]')
        if [ "$file" != "$lowercase_file" ]; then
            mv "$file" "$lowercase_file"
        fi
    fi
done

# add all the filenames in audio/sfx/*.wav to sfx.txt
# YOUR CODE HERE

# generate the sfx.h
FILENAME="sfx.h";
echo "#pragma once" > $FILENAME;
ct=0; 
for i in `cat sfx.txt`; do 
    a=$(echo SFX_$i $ct | sed 's/.wav//' | tr '[:lower:]' '[:upper:]'); 
    echo "#define" $a; 
    ct=$((ct+1)); 
done >> $FILENAME;


