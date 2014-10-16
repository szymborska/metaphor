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

#ifndef METAPHOR_DEBUG_H
#define METAPHOR_DEBUG_H

#include <stdio.h>
#include <fcntl.h>

void read_debugging_environment(void);
unsigned int get_debugging_flag(void);
char *get_debug_filename(void);

#define debug_print(fmt, ...) \
        do { read_debugging_environment(); if (get_debugging_flag() && get_debug_filename() == NULL) { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } else if ((get_debugging_flag()) && (get_debug_filename() != NULL)) {FILE * file_des = fopen(get_debug_filename(), "a+"); fprintf(file_des, fmt, __VA_ARGS__); fflush(file_des); fclose(file_des); } } while (0)

#endif
