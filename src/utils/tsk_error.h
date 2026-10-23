// SPDX-License-Identifier: GPL-3.0-only
#ifndef TSK_ERROR_H
#define TSK_ERROR_H
#include <string>
#include <tsk/libtsk.h>

inline std::string tsk_error_message()
{
    const char *message = tsk_error_get();
    return message ? message : "unknown TSK error";
}
#endif
