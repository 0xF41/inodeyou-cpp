#include "populate_fs_inodes.h"
#include "fs_walk_path.h"
#include <sys/stat.h>
#include <stdexcept>
void populate_fs_inodes(const char *path, InodePaths &paths)
{
    struct stat st;
    if (stat(path, &st) || !S_ISDIR(st.st_mode))
        throw std::runtime_error("Starting path is not an accessible directory");
    fs_walk_path(path, st.st_dev, paths);
}
