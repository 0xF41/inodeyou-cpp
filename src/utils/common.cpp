#include "common.h"
#include <sys/stat.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <limits.h>

using namespace std;

/**
 * Syncs the filesystem to ensure all data is written to disk.
 */
void sync_filesystem()
{
    sync();
}

/**
 * Drops the filesystem caches to free up memory.
 */
void drop_caches()
{
    int drop_cache_status = system("sh -c 'echo 3 > /proc/sys/vm/drop_caches'");
    if (drop_cache_status != 0)
    {
        cerr << "Error: Failed to drop caches." << endl;
    }
}

/**
 * Check if the provided path is valid (directory or file).
 * @param path Path to check
 * @return True if the path is valid, false otherwise
 */
bool is_valid_path(const std::string &path)
{
    struct stat buffer;

    // Use stat to get information about the path
    if (stat(path.c_str(), &buffer) != 0)
    {
        cerr << "[!] Error: Failed to stat path '" << path << "'." << endl;
        return false;
    }

    // Check for valid file types (directory, regular file, symbolic link, or block device)
    if (S_ISDIR(buffer.st_mode) || S_ISREG(buffer.st_mode) || S_ISLNK(buffer.st_mode) || S_ISBLK(buffer.st_mode))
    {
        return true;
    }

    cerr << "[!] Error: Path '" << path << "' is not a valid directory, file, symbolic link, or block device." << endl;
    return false;
}

/**
 * Check if the provided argument is a valid path and copy it to the parameter.
 * @param param Parameter to store the path
 * @param arg Argument to check
 * @param err_msg Error message to display if the path is invalid
 * @return True if the path is valid, false otherwise
 */
bool checkArgv(std::string &param, const char *arg, const std::string &err_msg)
{
    if (arg == nullptr)
    {
        cerr << "[!] Error: " << err_msg << " argument is null." << endl;
        return false;
    }

    if (!is_valid_path(arg))
    {
        cerr << "[!] Error: " << err_msg << " path '" << arg << "' is invalid (does not exist or is not a file/directory)." << endl;
        return false;
    }

    param = arg; // Assign only after validation succeeds
    return true;
}