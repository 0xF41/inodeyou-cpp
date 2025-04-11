#include <iostream>
#include <cstdlib>

using namespace std;

void sync_filesystem() {
    int sync_status = system("/bin/sync");
    if (sync_status != 0) {
        cerr << "Error: Failed to execute sync." << endl;
    }
}

void drop_caches() {
    int drop_cache_status = system("sh -c 'echo 3 > /proc/sys/vm/drop_caches'");
    if (drop_cache_status != 0) {
        cerr << "Error: Failed to drop caches." << endl;
    }
}