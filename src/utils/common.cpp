#include "common.h"
#include <sys/stat.h>
#include <cstdlib>

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
    int drop_cache_status = system("sh -c 'echo 3 > /proc/sys/vm/drop_caches'");
    if (drop_cache_status != 0) {
        cerr << "Error: Failed to drop caches." << endl;
    }
}

/**
 * Check if the provided path is valid.
 * @param path Path to check
 * @return True if the path is valid, false otherwise
 */
bool is_valid_path(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

/**
 * Check if the provided argument is a valid path and copy it to the parameter.
 * @param param Parameter to store the path
 * @param arg Argument to check
 * @param err_msg Error message to display if the path is invalid
 * @return True if the path is valid, false otherwise
 */
bool checkArgv(char param[MAX_BUF], char *arg, char *err_msg) {
    strncpy(param, arg, MAX_BUF - 1);
    param[MAX_BUF - 1] = '\0';
    if (!is_valid_path(param)) {
        cerr << "Error: Invalid " << err_msg << " path provided: " << param << endl;
        return false;
    }
    return true;
}