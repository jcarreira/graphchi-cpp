
CXX       = g++
CC        = gcc
CXXFLAGS  = -Wall -std=c++17 -ggdb
CFLAGS    = -Wall -ggdb
#-fPIC
LDFLAGS   = -ldl
#-fPIC
DDC_PATH  = /data/joao/graphchi-cpp/external/ddc
INCLUDES  = -I$(DDC_PATH)/src/client/  -I$(DDC_PATH)
LINCLUDES = -L$(DDC_PATH)/src/client/ -L$(DDC_PATH)/src/common/ -L$(DDC_PATH)/src/utils/ -L$(DDC_PATH)/src/authentication/
LIBS      = -lclient -lcommon -lutils -lauthentication

all:
	$(CXX) $(CXXFLAGS) -fPIC $(INCLUDES) -o blade_layer.so -shared blade_layer.cpp $(LDFLAGS) $(LINCLUDES) $(LIBS) -libverbs -lrdmacm
	$(CC) $(CFLAGS) -fPIC -c -o interc.o interc.c -libverbs -lrdmacm
	$(CC) $(CFLAGS) -shared -o interc.so interc.o -ldl -libverbs -lrdmacm
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o copy_file copy_file.cpp $(LDFLAGS) $(LINCLUDES) $(LIBS) -libverbs -lrdmacm -pthread
	$(CC) $(CFLAGS) -fPIC -c -o interc_mem.o interc_mem.c
	$(CC) $(CFLAGS) -shared -o interc_mem.so interc_mem.o -ldl

clean:
	rm *.o *.so copy_file -rf
	
#$(CC) $(CFLAGS) -fPIC -c -o ssd_interc.o ssd_interc.c
#$(CC) $(CFLAGS) -shared -o ssd_interc.so ssd_interc.o -ldl
