#include <BladeFileClient.h>

#define GB (1024L*1024*1024)
#define SIZE (10*GB)

static const char* IP = "10.10.49.83";
static const char* PORT = "12345";

#define PATH "/data/joao/ligra/utils/my_edge_1M"

int main() {
    FILE* fin = fopen(PATH, "r");
    if (!fin)
        return -1;

    char*buf = new char[SIZE];
    if (!buf)
        return -1;

    puts("Reading file");
    int size_read = fread(buf, 1, SIZE, fin);

    sirius::BladeFileClient client;
    puts("Connecting..");
    client.connect(IP, PORT);
    puts("Allocating..");
    sirius::FileAllocRec allocRec = client.allocate(PATH, size_read);

    puts("Writing..");
    client.write_sync(allocRec, 0, size_read, buf);

    puts("Done..");
    return 0;
}
