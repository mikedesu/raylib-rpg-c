#!/usr/bin/env zsh

echo -e "$(scc -i cpp,h,py,sh,frag,html)\n" | zstd -19 >> stats/stats.txt.zstd


