
TMP=tmp
BIN=bin

#OPT=-O3
#OPT=-g
OPT="-O3 -g"

mkdir -p $TMP
mkdir -p $BIN

gcc $OPT -c src/crude-xa.c -o $TMP/crude-xa.o
g++ $OPT -c crude-xa-test.cpp -o $TMP/crude-xa-test.o
g++ $OPT $TMP/crude-xa.o $TMP/crude-xa-test.o -o $BIN/crude-xa-test
