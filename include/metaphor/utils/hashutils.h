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

#ifndef METAPHOR_HASHUTILS_H
#define METAPHOR_HASHUTILS_H

#include <metaphor/third-party/uthash.h>

struct hash_entry {
	char key[100];
	char value[100];
	UT_hash_handle hh;
};

int free_dictionary(struct hash_entry *dictionary);
struct hash_entry *add_hash_int(struct hash_entry *dictionary, char *key,
				int value);
struct hash_entry *add_hash_string(struct hash_entry *dictionary, char *key,
				   char *value);

#endif
