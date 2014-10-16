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

#ifndef METAPHOR_TYPES_H
#define METAPHOR_TYPES_H

#ifdef _WIN32
#ifdef _WIN64
#endif
#elif __APPLE__
#if TARGET_OS_MAC
#else
#endif
#elif __linux

#include <sys/types.h>

#elif __unix
#elif __posix
#endif

#endif
