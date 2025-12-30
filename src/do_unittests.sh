
make clean && CXXFLAGS=" -DDEBUG=1 -DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DMUSIC_OFF" make -j8 tests && ./tests
