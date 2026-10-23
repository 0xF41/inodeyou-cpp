// SPDX-License-Identifier: GPL-3.0-only
#ifndef INODEYOU_SYSCALLS_H
#define INODEYOU_SYSCALLS_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

inline int retry_open(const char *path, int flags)
{
    int fd;
    do {
        fd = ::open(path, flags);
    } while (fd < 0 && errno == EINTR);
    return fd;
}

inline int retry_openat(int dirfd, const char *path, int flags)
{
    int fd;
    do {
        fd = ::openat(dirfd, path, flags);
    } while (fd < 0 && errno == EINTR);
    return fd;
}

inline int retry_fstat(int fd, struct stat *st)
{
    int result;
    do {
        result = ::fstat(fd, st);
    } while (result != 0 && errno == EINTR);
    return result;
}

inline int retry_fstatat(int dirfd, const char *path, struct stat *st, int flags)
{
    int result;
    do {
        result = ::fstatat(dirfd, path, st, flags);
    } while (result != 0 && errno == EINTR);
    return result;
}

inline int retry_statx(int fd, const char *path, int flags, unsigned int mask,
                       struct statx *stx)
{
    int result;
    do {
        result = ::statx(fd, path, flags, mask, stx);
    } while (result != 0 && errno == EINTR);
    return result;
}

inline int retry_syncfs(int fd)
{
    int result;
    do {
        result = ::syncfs(fd);
    } while (result != 0 && errno == EINTR);
    return result;
}

inline dirent *retry_readdir(DIR *dir)
{
    dirent *entry;
    do {
        errno = 0;
        entry = ::readdir(dir);
    } while (!entry && errno == EINTR);
    return entry;
}
#endif
