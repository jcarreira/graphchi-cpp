
CXX=g++
CFLAGS=-O3 -Wall
#-ggdb
LDFLAGS=-ldl -fPIC
PROJ=wrapper

all:
	$(CXX) $(CFLAGS) -o $(PROJ).so -shared $(PROJ).cpp $(LDFLAGS)
	gcc -shared -o interc.so interc.o -ldl
	gcc -fPIC -c -o interc.o interc.cpp
