
CXX       = g++
CC        = gcc
CXXFLAGS  = -Wall -std=c++11 -ggdb
CFLAGS    = -Wall -ggdb
#-fPIC
LDFLAGS   = -ldl
#-fPIC
INCLUDES  = -I/data/joao/ddc/src/client/  -I/data/joao/ddc/
LINCLUDES = -L/data/joao/ddc/src/client/ -L/data/joao/ddc/src/common/ -L/data/joao/ddc/src/utils/
LIBS      = -lclient -lcommon -lutils
PROJ      = wrapper

all:
	$(CXX) $(CXXFLAGS) -fPIC -o $(PROJ).so -shared $(PROJ).cpp $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -fPIC $(INCLUDES) -o blade_layer.so -shared blade_layer.cpp $(LDFLAGS) $(LINCLUDES) $(LIBS) -libverbs -lrdmacm
	$(CC) $(CFLAGS) -fPIC -c -o interc.o interc.c -libverbs -lrdmacm
	$(CC) $(CFLAGS) -shared -o interc.so interc.o -ldl -libverbs -lrdmacm

clean:
	rm *.o *.so -rf
