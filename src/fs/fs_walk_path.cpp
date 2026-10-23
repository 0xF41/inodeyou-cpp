// SPDX-License-Identifier: GPL-3.0-only
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "fs_walk_path.h"
#include "../utils/unique_fd.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <memory>
#include <set>
#include <stdexcept>
#include <utility>

namespace {

constexpr unsigned MAX_DEPTH = 256;

struct DirCloser {
    void operator()(DIR *dir) const { if (dir) closedir(dir); }
};

using DirectoryId = std::pair<dev_t, ino_t>;

[[noreturn]] void fail(const std::string &message)
{
    throw std::runtime_error(message + ": " + std::strerror(errno));
}

DirectoryId identity(const struct stat &st)
{
    return {st.st_dev, st.st_ino};
}

unsigned long long mount_id(int fd)
{
    struct statx stx;
    if (statx(fd, "", AT_EMPTY_PATH, STATX_MNT_ID, &stx) != 0)
        fail("Cannot read directory mount identity");
    if (!(stx.stx_mask & STATX_MNT_ID) || stx.stx_mnt_id == 0)
        throw std::runtime_error("Mount identity unavailable; refusing ambiguous traversal");
    return stx.stx_mnt_id;
}

void walk_dir(int fd, const std::string &path, dev_t device,
              unsigned long long root_mount, InodePaths &paths,
              std::set<DirectoryId> &ancestors, std::size_t &entries,
              unsigned depth)
{
    UniqueFd owner(fd);
    if (depth > MAX_DEPTH)
        throw std::runtime_error("Mounted directory depth limit exceeded");

    DIR *raw = fdopendir(owner.get());
    if (!raw)
        fail("Cannot enumerate mounted directory: " + path);
    owner.release(); // fdopendir owns the descriptor
    std::unique_ptr<DIR, DirCloser> dir(raw);
    const int dir_fd = dirfd(dir.get());
    if (dir_fd < 0)
        fail("Cannot obtain directory descriptor: " + path);

    while (true) {
        errno = 0;
        dirent *entry = readdir(dir.get());
        if (!entry) {
            if (errno)
                fail("Mounted enumeration failed: " + path);
            break;
        }
        if (!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
            continue;

        const std::string full = join_path(path, entry->d_name);
        struct stat st;
        if (fstatat(dir_fd, entry->d_name, &st, AT_SYMLINK_NOFOLLOW) != 0)
            fail("Cannot stat mounted entry: " + full);
        if (++entries > MAX_INVENTORY_ENTRIES)
            throw std::runtime_error("Inventory entry limit exceeded");

        paths[static_cast<Inode>(st.st_ino)].insert(full);

        if (!S_ISDIR(st.st_mode)) {
            if (st.st_dev != device)
                throw std::runtime_error("Nested filesystem encountered: " + full);
            continue;
        }

        const DirectoryId child_id = identity(st);
        if (!ancestors.insert(child_id).second)
            throw std::runtime_error("Mounted directory cycle detected: " + full);

        UniqueFd child(openat(dir_fd, entry->d_name,
                              O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW));
        if (child.get() < 0) {
            ancestors.erase(child_id);
            fail("Cannot open mounted directory: " + full);
        }

        struct stat opened;
        if (fstat(child.get(), &opened) != 0) {
            ancestors.erase(child_id);
            fail("Cannot inspect mounted directory: " + full);
        }
        if (identity(opened) != child_id || !S_ISDIR(opened.st_mode)) {
            ancestors.erase(child_id);
            throw std::runtime_error("Mounted directory identity changed during traversal: " + full);
        }
        if (opened.st_dev != device) {
            ancestors.erase(child_id);
            throw std::runtime_error("Nested filesystem encountered: " + full);
        }
        if (mount_id(child.get()) != root_mount) {
            ancestors.erase(child_id);
            throw std::runtime_error("Mount boundary encountered: " + full);
        }

        walk_dir(child.release(), full, device, root_mount, paths, ancestors,
                 entries, depth + 1);
        ancestors.erase(child_id);
    }
}

} // namespace

void fs_walk_fd(int fd, const std::string &path, dev_t expected_dev,
                ino_t expected_ino, InodePaths &paths)
{
    UniqueFd owner(fd);
    struct stat st;
    if (fstat(owner.get(), &st) != 0 || !S_ISDIR(st.st_mode))
        fail("Starting path is not an accessible directory: " + path);
    if (st.st_dev != expected_dev || st.st_ino != expected_ino)
        throw std::runtime_error("Starting directory identity changed: " + path);

    std::set<DirectoryId> ancestors;
    ancestors.insert(identity(st));
    std::size_t entries = 0;
    const unsigned long long root_mount = mount_id(owner.get());
    walk_dir(owner.release(), path, st.st_dev, root_mount, paths, ancestors,
             entries, 0);
}

void fs_walk_root(const std::string &path, InodePaths &paths)
{
    UniqueFd fd(open(path.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW));
    if (fd.get() < 0)
        fail("Cannot open mounted directory: " + path);

    struct stat st;
    if (fstat(fd.get(), &st) != 0 || !S_ISDIR(st.st_mode))
        fail("Starting path is not an accessible directory: " + path);

    fs_walk_fd(fd.release(), path, st.st_dev, st.st_ino, paths);
}
