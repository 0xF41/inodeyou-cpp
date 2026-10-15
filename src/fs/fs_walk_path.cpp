#include "fs_walk_path.h"
#include <dirent.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <memory>
#include <stdexcept>

void fs_walk_path(const std::string &path, dev_t device, InodePaths &paths, unsigned depth)
{
    if (depth > 256) throw std::runtime_error("Mounted directory depth limit exceeded");
    auto close_dir = [](DIR *value) { closedir(value); };
    std::unique_ptr<DIR, decltype(close_dir)> dir(opendir(path.c_str()), close_dir);
    if (!dir) throw std::runtime_error("Cannot open mounted directory: " + path);
    while (true) {
        errno = 0;
        dirent *entry = readdir(dir.get());
        if (!entry) {
            if (errno) throw std::runtime_error("Mounted enumeration failed: " + path);
            break;
        }
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        const auto full = join_path(path, entry->d_name);
        struct stat st;
        // lstat handles DT_UNKNOWN and does not follow symlinks out of scope.
        if (lstat(full.c_str(), &st)) throw std::runtime_error("Cannot stat mounted entry: " + full);
        if (st.st_dev != device)
            throw std::runtime_error("Nested filesystem encountered; choose a narrower scan directory: " + full);
        paths[static_cast<Inode>(st.st_ino)].insert(full);
        if (S_ISDIR(st.st_mode)) fs_walk_path(full, device, paths, depth + 1);
    }
}
