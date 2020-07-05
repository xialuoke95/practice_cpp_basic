readelf -d test
export LD_LIBRARY_PATH=./pos2
strace ./test 
ldd ./test