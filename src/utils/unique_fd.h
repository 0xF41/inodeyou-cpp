// SPDX-License-Identifier: GPL-3.0-only
#ifndef UNIQUE_FD_H
#define UNIQUE_FD_H
#include <unistd.h>

class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) : fd_(fd) {}
    ~UniqueFd() { reset(); }
    UniqueFd(const UniqueFd &) = delete;
    UniqueFd &operator=(const UniqueFd &) = delete;

    int get() const { return fd_; }
    int release()
    {
        int fd = fd_;
        fd_ = -1;
        return fd;
    }
    void reset(int fd = -1)
    {
        if (fd_ >= 0)
            ::close(fd_);
        fd_ = fd;
    }

private:
    int fd_;
};
#endif
