if [[ -f game ]]; then
    echo $(date +%s),$(ls -al game | awk '{print $5}'),$(cat *.h *.c | egrep -v "^[[:space:]]*//" | egrep -v "^$" | wc -l) >> build_size.csv
fi

