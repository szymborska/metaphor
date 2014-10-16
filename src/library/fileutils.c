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
#include <stdlib.h>
#include <errno.h>

#include <metaphor/utils/fileutils.h>
#include <metaphor/utils/stringutils.h>
#include <metaphor/utils/debug.h>

#pragma GCC diagnostic push	// require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

char *
read_nonbinary_file(char *filepath)
{

	FILE *file = fopen(filepath, "rb");

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *data = malloc(file_size + 1);
	int bytes_read = fread(data, 1, file_size, file);
 	if (bytes_read < file_size) {
		printf("Failed to read file. Only read %d bytes.\n", bytes_read);
		exit(1);
 	}
	fclose(file);
	data[bytes_read] = '\0';

	return data;
}

char *
get_next_sub_path(char *path)
{

	char *subpath = path;
	char *path_pointer = strchr(path, '/');
	if (path_pointer != NULL) {
		path_pointer = path_pointer + 1;
		int distance = (int) (path_pointer - path);
		if (distance > 0) {
			subpath =
			    safe_substring_copy(8096, path, 0, distance - 2);
		}
	} else {
		subpath = safe_string_copy(8096, path);
	}

	return subpath;
}

char *
get_leaf(const char *path)
{

	char *leaf_name;
	char *path_pointer = strrchr(path, '/');
	if (path_pointer != NULL) {
		path_pointer = path_pointer + 1;
		int distance = (int) (path_pointer - path);
		if (distance > 0) {
			leaf_name = safe_string_copy(8096, path_pointer);
		} else {
			leaf_name = safe_string_copy(8096, (char *) path);
		}
	} else {
		leaf_name = safe_string_copy(8096, (char *) path);
	}

	return leaf_name;
}
