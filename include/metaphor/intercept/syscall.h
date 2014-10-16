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

#ifndef METAPHOR_SYSCALL_H
#define METAPHOR_SYSCALL_H

#include <metaphor/os/types.h>
#include <metaphor/third-party/uthash.h>

// Standard posix calls 
int m_unlink(const char *pathname);
ssize_t m_read(int fd, void *buf, size_t count);
int m_open(const char *pathname, int flags);
off_t m_lseek(int fd, off_t offset, int whence);
int m_close(int fd);
int m_pause(void);

#endif
