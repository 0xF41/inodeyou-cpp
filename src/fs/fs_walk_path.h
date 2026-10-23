// SPDX-License-Identifier: GPL-3.0-only
#ifndef FS_WALK_PATH_H
#define FS_WALK_PATH_H
#include "../utils/inode_utils.h"
#include <string>

// Walk the mounted view with descriptor-relative, no-follow directory traversal.
void fs_walk_root(const std::string &path, InodePaths &paths);

#endif
