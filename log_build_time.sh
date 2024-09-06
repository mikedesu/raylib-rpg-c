make clean;
{ time make } 2>time.txt;
echo $(date +%s),$(awk '{print $8}' time.txt) >> build_time.csv;
rm -rfv time.txt;
