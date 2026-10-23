// SPDX-License-Identifier: GPL-3.0-only
#include "tsk_walk_path.h"
#include <memory>
#include <cstring>
#include <stdexcept>

void tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T inode, const std::string &path,
                   InodePaths &paths, std::set<Inode> &ancestors, unsigned depth)
{
    if (depth > 256 || !ancestors.insert(static_cast<Inode>(inode)).second)
        throw std::runtime_error("TSK directory cycle/depth limit encountered");
    std::unique_ptr<TSK_FS_DIR, decltype(&tsk_fs_dir_close)> dir(
        tsk_fs_dir_open_meta(fs, inode), tsk_fs_dir_close);
    if (!dir) throw std::runtime_error("TSK cannot open directory: " + path);
    for (size_t i = 0; i < tsk_fs_dir_getsize(dir.get()); ++i) {
        std::unique_ptr<TSK_FS_FILE, decltype(&tsk_fs_file_close)> file(
            tsk_fs_dir_get(dir.get(), i), tsk_fs_file_close);
        if (!file || !file->name || !file->name->name)
            throw std::runtime_error("TSK returned an invalid directory entry");
        if (!(file->name->flags & TSK_FS_NAME_FLAG_ALLOC)) continue;
        const char *name = file->name->name;
        if (!strcmp(name, ".") || !strcmp(name, "..") ||
            !strcmp(name, "$OrphanFiles")) continue;
        if (!file->meta) throw std::runtime_error("TSK entry metadata unavailable");
        if (file->meta->type == TSK_FS_META_TYPE_VIRT ||
            file->meta->type == TSK_FS_META_TYPE_VIRT_DIR) continue;
        const auto full = join_path(path, name);
        const auto number = static_cast<Inode>(file->meta->addr);
        paths[number].insert(full);
        if (file->meta->type == TSK_FS_META_TYPE_DIR)
            tsk_walk_path(fs, file->meta->addr, full, paths, ancestors, depth + 1);
    }
    ancestors.erase(static_cast<Inode>(inode));
}
