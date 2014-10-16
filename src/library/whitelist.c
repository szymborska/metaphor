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

#include <metaphor/os/whitelist.h>
#include <metaphor/utils/stringutils.h>
#include <metaphor/third-party/utlist.h>

typedef struct whitelist_path {
	char *content;
	struct whitelist_path *next;
} whitelist_path;

whitelist_path *whitelist_paths = NULL;

int
set_whitelist(char *filepaths[], int size)
{

	int i;
	for (i = 0; i < size; i++) {
		char *filepath = filepaths[i];
		char *copied_path = safe_string_copy(8096, filepath);
		struct whitelist_path *path =
		    malloc(sizeof (struct whitelist_path));
		path->content = copied_path;
		LL_APPEND(whitelist_paths, path);
	}

	return 0;
}

int
check_whitelist(const char *pathname)
{

	struct whitelist_path *path;
	struct whitelist_path *tmp;
	int found_a_match = 0;
	DL_FOREACH_SAFE(whitelist_paths, path, tmp) {
		char *file_path = path->content;
		if (strncmp(pathname, file_path, strlen(file_path)) == 0) {
			found_a_match = 1;
		}
	}
	return found_a_match;
}

void
delete_whitelist(void)
{
	if (whitelist_paths != NULL) {
		struct whitelist_path *path;
		struct whitelist_path *tmp;
		DL_FOREACH_SAFE(whitelist_paths, path, tmp) {
			LL_DELETE(whitelist_paths, path);
			free(path->content);
			free(path);
		}
	}
}
