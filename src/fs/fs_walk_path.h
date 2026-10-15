#ifndef FS_WALK_PATH_H
#define FS_WALK_PATH_H
#include "../utils/inode_utils.h"
#include <sys/types.h>
void fs_walk_path(const std::string &path, dev_t device, InodePaths &paths, unsigned depth = 0);
#endif
