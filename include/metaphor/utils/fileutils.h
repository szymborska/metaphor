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

#ifndef METAPHOR_FILEUTILS_H
#define METAPHOR_FILEUTILS_H

char *read_nonbinary_file(char *filepath);
char *get_next_sub_path(char *path);
char *get_leaf(const char *path);

#endif
