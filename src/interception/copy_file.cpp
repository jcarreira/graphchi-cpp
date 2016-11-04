#include <BladeFileClient.h>

#define GB (1024L*1024*1024)
#define SIZE (10*GB)

int main() {
    FILE* fin = fopen(PATH, "r");
    if (!fin)
        return -1;

    char*buf = new char[SIZE];
    if (!buf)
        return -1;

    int size_read = fread(buf, 1, SIZE, fin);

    sirius::BladeFileClient client;
    client.connect("10.10.49.83", "12345");
    sirius::FileAllocRec allocRec = client.allocate(PATH, size_read);

    client.write_sync(buf, size_read);

    return 0;
}
