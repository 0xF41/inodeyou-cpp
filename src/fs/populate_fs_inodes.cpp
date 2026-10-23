// SPDX-License-Identifier: GPL-3.0-only
#include "populate_fs_inodes.h"
#include "fs_walk_path.h"

void populate_fs_inodes(const char *path, InodePaths &paths)
{
    fs_walk_root(path, paths);
}
