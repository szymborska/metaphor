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

#include <stdio.h>
#include <bsd/string.h>

#include <metaphor/utils/hashutils.h>
#include <metaphor/utils/stringutils.h>

int
free_dictionary(struct hash_entry *dictionary)
{

	struct hash_entry *entry = NULL;
	struct hash_entry *tmp = NULL;

	/* free the hash table contents */
	HASH_ITER(hh, dictionary, entry, tmp) {
		HASH_DEL(dictionary, entry);
		free(entry);
	}

	return 0;
}

struct hash_entry *
add_hash_int(struct hash_entry *dictionary, char *key, int value)
{

	struct hash_entry *entry;
	char number_string[10];

	snprintf(number_string, 10, "%d", value);
	entry = (struct hash_entry *) malloc(sizeof (struct hash_entry));
	// Char arrays so strlcpy instead of safe_string_copy
	strlcpy(entry->key, key, sizeof (entry->key));
	strlcpy(entry->value, number_string, sizeof (entry->value));
	HASH_ADD_STR(dictionary, key, entry);

	return dictionary;
}

struct hash_entry *
add_hash_string(struct hash_entry *dictionary, char *key, char *value)
{

	struct hash_entry *entry =
	    (struct hash_entry *) malloc(sizeof (struct hash_entry));
	// Char arrays so strlcpy instead of safe_string_copy
	strlcpy(entry->key, key, sizeof (entry->key));
	strlcpy(entry->value, value, sizeof (entry->value));
	HASH_ADD_STR(dictionary, key, entry);

	return dictionary;
}
