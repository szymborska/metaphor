/*******************************************************************************
 *All of the software in metaphor unless otherwise indicated is available under
 * the following licenses:
 *
 * Apache 2.0 License
 * New BSD License
 * LGPL 3.0 License
 * MIT License
 *
 * For more information, please consult the licenses. 
 *
*/

#ifndef METAPHOR_INTERCEPT_CALLS_H
#define METAPHOR_INTERCEPT_CALLS_H

/* 
    A macro that can be easily called to generate all of the overriden
    syscalls used by a metaphor.
*/

#include <metaphor/os/os.h>

#define SYSCALL_DEFINITIONS(METAPHOR_FUNCTION) \
\
extern int m_unlink(const char *pathname); \
int unlink(const char *pathname); \
int unlink(const char *pathname) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_unlink(pathname); \
} \
\
extern off_t m_lseek(int fd, off_t offset, int whence); \
off_t lseek(int fd, off_t offset, int whence); \
off_t lseek(int fd, off_t offset, int whence) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_lseek(fd, offset, whence); \
} \
\
extern DIR *m_opendir (const char *__name) __nonnull ((1)); \
DIR * opendir(const char * pathname); \
DIR * opendir(const char * pathname) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_opendir(pathname); \
} \
\
extern int m_closedir(DIR * dirp); \
int closedir(DIR *dirp); \
int closedir(DIR *dirp) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_closedir(dirp); \
} \
\
extern struct dirent *m_readdir(DIR *dirp); \
struct dirent *readdir(DIR *dirp); \
struct dirent *readdir(DIR *dirp) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_readdir(dirp); \
} \
\
extern int m_open(const char *pathname, int flags); \
int open(const char *pathname, int flags); \
int open(const char *pathname, int flags) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_open(pathname, flags); \
} \
\
extern ssize_t m_read(int fd, void *buf, size_t count); \
ssize_t read(int fd, void *buf, size_t count); \
ssize_t read(int fd, void *buf, size_t count) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_read(fd, buf, count); \
} \
\
extern int m_close(int fd); \
int close(int fd); \
int close(int fd) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_close(fd); \
} \
\
extern int m_xstat (int format, const char *path, struct stat *buf); \
int __xstat (int format, const char *path, struct stat *buf); \
int __xstat (int format, const char *path, struct stat *buf) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_xstat(format, path, buf); \
} \
\
extern int m_lxstat(int format, const char *path, struct stat *buf); \
int __lxstat(int format, const char *path, struct stat *buf); \
int __lxstat(int format, const char *path, struct stat *buf) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_lxstat(format, path, buf); \
} \
\
extern int m_fxstat(int format, int fd, struct stat *buf); \
int __fxstat(int format, int fd, struct stat *buf); \
int __fxstat(int format, int fd, struct stat *buf) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_fxstat(format, fd, buf); \
} \
\
extern int m_openat(int dirfd, const char *pathname, int flags); \
int openat(int dirfd, const char *pathname, int flags); \
int openat(int dirfd, const char *pathname, int flags) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_openat(dirfd, pathname, flags); \
} \
\
extern int m_pause(void); \
int pause(void); \
int pause(void) { \
\
    set_remap_resources_function(METAPHOR_FUNCTION); \
    if (! get_initializing_resources() ) { \
        remap_resources(); \
    } \
    return m_pause(); \
} \

#endif
