// SPDX-License-Identifier: GPL-3.0-only
#ifndef FS_WALK_PATH_H
#define FS_WALK_PATH_H
#include "../utils/inode_utils.h"
#include <string>
#include <sys/types.h>

// Open and verify a path before walking the mounted view.
void fs_walk_root(const std::string &path, InodePaths &paths);

// Walk from an already-open descriptor. Takes ownership of fd and verifies
// that it still refers to the expected directory identity.
void fs_walk_fd(int fd, const std::string &path, dev_t expected_dev,
                ino_t expected_ino, InodePaths &paths);

#endif
