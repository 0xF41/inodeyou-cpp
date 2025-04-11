#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>


using namespace std;

/**
 * Syncs the filesystem to ensure all data is written to disk.
 */
void sync_filesystem() {
    int sync_status = system("/bin/sync");
    if (sync_status != 0) {
        cerr << "Error: Failed to execute sync." << endl;
    }
}

/**
 * Drops the filesystem caches to free up memory.
 */
void drop_caches() {
    int fd = open("/proc/sys/vm/drop_caches", O_WRONLY);
    if (fd == -1) {
        cerr << "Error: Failed to open /proc/sys/vm/drop_caches." << endl;
        return;
    }
    if (write(fd, "3", 1) == -1) {
        cerr << "Error: Failed to write to /proc/sys/vm/drop_caches." << endl;
    }
    close(fd);
}