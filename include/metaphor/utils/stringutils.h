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

#ifndef METAPHOR_STRINGUTILS_H
#define METAPHOR_STRINGUTILS_H

extern int ESTRINGTRUNCATED;

char *safe_convert_int_to_string(int value);
int safe_convert_string_to_int(char *value);
char *safe_string_copy(int max_size, char *string);
char *safe_substring_copy(int max_size, char *string, int beginning, int end);
char *safe_join_strings(int max_size, int count, ...);

#endif
