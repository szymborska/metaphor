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

#ifndef METAPHOR_WHITELIST_H
#define METAPHOR_WHITELIST_H

int set_whitelist(char *filepaths[], int size);
int check_whitelist(const char *pathname);
void delete_whitelist(void);

#endif
