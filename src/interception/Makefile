
CXX       = g++
CC        = gcc
CXXFLAGS  = -Wall -std=c++17 -ggdb
CFLAGS    = -Wall -ggdb
#-fPIC
LDFLAGS   = -ldl
#-fPIC
INCLUDES  = -I/data/joao/ddc/src/client/  -I/data/joao/ddc/
LINCLUDES = -L/data/joao/ddc/src/client/ -L/data/joao/ddc/src/common/ -L/data/joao/ddc/src/utils/ -L/data/joao/ddc/src/authentication/
LIBS      = -lclient -lcommon -lutils -lauthentication

all:
	$(CXX) $(CXXFLAGS) -fPIC $(INCLUDES) -o blade_layer.so -shared blade_layer.cpp $(LDFLAGS) $(LINCLUDES) $(LIBS) -libverbs -lrdmacm
	$(CC) $(CFLAGS) -fPIC -c -o interc.o interc.c -libverbs -lrdmacm
	$(CC) $(CFLAGS) -shared -o interc.so interc.o -ldl -libverbs -lrdmacm
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o copy_file copy_file.cpp $(LDFLAGS) $(LINCLUDES) $(LIBS) -libverbs -lrdmacm -pthread
	$(CC) $(CFLAGS) -fPIC -c -o ssd_interc.o ssd_interc.c
	$(CC) $(CFLAGS) -shared -o ssd_interc.so ssd_interc.o -ldl

clean:
	rm *.o *.so -rf
