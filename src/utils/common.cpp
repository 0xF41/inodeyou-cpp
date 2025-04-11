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
 * Check if the provided path is valid (directory or file).
 * @param path Path to check
 * @return True if the path is valid, false otherwise
 */
bool is_valid_path(const std::string &path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode) || S_ISREG(buffer.st_mode);
}

/**
 * Check if the provided argument is a valid path and copy it to the parameter.
 * @param param Parameter to store the path
 * @param arg Argument to check
 * @param err_msg Error message to display if the path is invalid
 * @return True if the path is valid, false otherwise
 */
bool checkArgv(std::string &param, const char *arg, const std::string &err_msg) {
    param = arg;
    if (!is_valid_path(param)) {
        cerr << "Error: Invalid " << err_msg << " path provided: " << param << endl;
        return false;
    }
    return true;
}