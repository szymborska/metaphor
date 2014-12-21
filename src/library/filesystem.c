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

#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include <metaphor/third-party/uthash.h>
#include <metaphor/third-party/utlist.h>
#include <metaphor/os/filesystem.h>
#include <metaphor/os/whitelist.h>
#include <metaphor/utils/fileutils.h>
#include <metaphor/utils/stringutils.h>
#include <metaphor/utils/debug.h>

struct m_file *root_filesystem = NULL;
struct m_open_file *open_files = NULL;
struct m_directory *open_directories = NULL;
unsigned int current_fd;
long unsigned int current_directory_pointer_count;
unsigned int inode_number;

long unsigned int
get_currect_directory_pointer_count(void)
{
	return current_directory_pointer_count;
}

void
set_current_directory_pointer_count(long unsigned int new_count)
{
	current_directory_pointer_count = new_count;
}

unsigned int
get_current_fd(void)
{
	return current_fd;
}

void
set_current_fd(unsigned int new_fd)
{
	current_fd = new_fd;
}

void
set_root_filesystem(struct m_file *file)
{

	root_filesystem = file;
}

struct m_file *
get_root_filesystem(void)
{

	return root_filesystem;
}

struct m_open_file *
get_open_files(void)
{

	return open_files;
}

void
set_open_files(struct m_open_file *new_open_files)
{
	open_files = new_open_files;
}

struct m_directory *
get_open_directories(void)
{

	return open_directories;
}

void
set_open_directories(struct m_directory *new_open_directories)
{

	open_directories = new_open_directories;
}

int
initialize_filesystem(void)
{

	struct m_file *filesystem = new_filesystem();
	set_root_filesystem(filesystem);
	current_fd = 100000;
	current_directory_pointer_count = 100000;
	inode_number = 100000;

	return 0;
}

struct m_file *
new_filesystem(void)
{

	void *data = NULL;
	struct m_file *root_file =
	    new_file_with_parameters("/", "/",  (uintptr_t) NULL, data, 0, 0, 1, 1, NULL, NULL, NULL);

	return root_file;
}

void
delete_filesystem(struct m_file *in_file)
{

	if (in_file->files != NULL) {
		struct m_file *entry = NULL;
		struct m_file *tmp = NULL;

		HASH_ITER(hh, in_file->files, entry, tmp) {
			HASH_DEL(in_file->files, entry);
			if (entry->is_directory == 1) {
				delete_filesystem(entry);
			} else {

				if (entry->data != NULL) {
					free(entry->data);
				}
				if (entry->name != NULL) {
					free(entry->name);
				}
				if (entry->path != NULL) {
					free(entry->path);
				}

				free(entry);
			}
		}
	}

	if (in_file->data != NULL) {
		free(in_file->data);
	}
	if (in_file->name != NULL) {
		free(in_file->name);
	}
	if (in_file->path != NULL) {
		free(in_file->path);
	}

	free(in_file);
	// delete_whitelist();
}

/*
 * The method copies the name and path into a new string but does
 * *not* copy the data to avoid copying a potentially large
 * set of data.
 */
struct m_file *new_file_with_parameters(char *name, char *path, uintptr_t metadata, void *data,
					int length, int permissions,
					int is_directory, int is_root,
                                        READ_FUNCTION read_function,
                                        SIZE_FUNCTION size_function,
                                        CLEANUP_FUNCTION cleanup_function)
{

	debug_print("Creating new file %s with name %s with data %s.\n", path,
		    name, (char *) data);
	inode_number = inode_number + 1;
	struct m_file *file = (struct m_file *) malloc(sizeof (struct m_file));
	file->name = safe_string_copy(8096, name);
	file->path = safe_string_copy(8096, path);
	file->data = data;
	file->length = length;
        file->metadata = metadata;
	file->permissions = permissions;
	file->is_directory = is_directory;
	file->is_root = is_root;
	file->files = NULL;
	file->inode = inode_number;
        file->read_function = read_function;
        file->size_function = size_function;
        file->cleanup_function = cleanup_function;

	return file;
}

struct m_file * new_file_with_dynamic_data(struct m_file *filesystem, char *path, uintptr_t metadata, 
 		 		 READ_FUNCTION read_function, SIZE_FUNCTION size_function, 
                                 CLEANUP_FUNCTION cleanup_function)
{
	char *leaf_name = get_leaf(path);
	debug_print("NEW FILE with READ FUNCTION: The leaf name of %s is %s.\n", path, leaf_name);
	struct m_file *file;
	struct m_file *parent_directory =
	    get_directory_of_file(filesystem, path, 1);
	file = new_file_with_parameters(leaf_name, path, metadata, NULL, 0, 0, 0, 0, read_function, size_function, cleanup_function);
	HASH_ADD_STR(parent_directory->files, name, file);

	return file;
}

struct m_file *
new_file(struct m_file *filesystem, char *path)
{

	struct m_file *file = new_file_with_static_data(filesystem, path, NULL, 0);
	return file;
}

struct m_file *
new_file_with_static_data(struct m_file *filesystem, char *path, void *data,
		   int length)
{

	char *leaf_name = get_leaf(path);
	debug_print("NEW FILE: The leaf name of %s is %s.\n", path, leaf_name);
	struct m_file *file;
	struct m_file *parent_directory =
	    get_directory_of_file(filesystem, path, 1);
	file = new_file_with_parameters(leaf_name, path, (uintptr_t) NULL, data, length, 0, 0, 0, NULL, NULL, NULL);
	HASH_ADD_STR(parent_directory->files, name, file);

	return file;
}

struct m_file *
get_file(struct m_file *filesystem, char *path)
{

	char *leaf_name = get_leaf(path);
	struct m_file *file = NULL;
	struct m_file *parent_directory =
	    get_directory_of_file(filesystem, path, 0);
	if (parent_directory != NULL) {
		HASH_FIND_STR(parent_directory->files, leaf_name, file);
	}
	free(leaf_name);

	return file;
}

struct m_file *
get_directory_of_file(struct m_file *filesystem, char *path,
		      int should_create_directories)
{

	char *remaining_path;
	if (path[0] == '/') {
		remaining_path =
		    safe_substring_copy(8096, path, 1, strlen(path));
	} else {
		remaining_path = safe_string_copy(8096, path);
	}
	char *full_remaining_path = remaining_path;
	debug_print("GET FILE: Remaining Path: %s.\n", remaining_path);

	struct m_file *current_directory = filesystem;

	int finished_directories = 0;
	while (!finished_directories) {
		char *subpath;
		struct m_file *file;
	
		subpath = get_next_sub_path(remaining_path);
		debug_print("The returned subpath is: %s.\n", subpath);
		if (strlen(remaining_path + strlen(subpath)) == 0) {
			remaining_path = remaining_path + strlen(subpath);
		} else {
			remaining_path = remaining_path + strlen(subpath) + 1;
		}
		debug_print("The remaining path is now: %s.\n", remaining_path);

		if (strlen(remaining_path) > 0) {
			debug_print("Remaining path is %s.\n", remaining_path);
	
			HASH_FIND_STR(current_directory->files, subpath, file);

			if (file) {
				debug_print
				    ("%s: Found the file, changing directory.\n",
				     "NEW FILE:");
			} else if (should_create_directories) {
				int path_length =
				    (int) (remaining_path -
					   full_remaining_path) - 1;
				char *full_directory_path =
				    safe_substring_copy(8096, path, 0,
							path_length);
				file =
				    new_file_with_parameters(subpath,
							     full_directory_path,
							     (uintptr_t) NULL, NULL, 0, 0, 1, 0,
                                                             NULL, NULL, NULL);
				free(full_directory_path);
				HASH_ADD_STR(current_directory->files, name,
					     file);
			} else {
				file = NULL;
				finished_directories = 1;
			}

			current_directory = file;
		} else {
			finished_directories = 1;
		}
		free(subpath);
	}
	free(full_remaining_path);

	return current_directory;
}

int
m_delete(struct m_file *filesystem, struct m_file *file_to_delete)
{

	char *path = file_to_delete->name;
	char *leaf_name = get_leaf(path);
	struct m_file *parent_directory =
	    get_directory_of_file(filesystem, path, 0);
	debug_print("Leaf name: %s.\n", leaf_name);
	if (parent_directory != NULL) {
		debug_print("Parent directory name: %s.\n",
			    parent_directory->name);
		struct m_file *file;
		HASH_FIND_STR(parent_directory->files, leaf_name, file);
		if (file) {
			HASH_DEL(parent_directory->files, file);

			if (file->data != NULL) {
				free(file->data);
			}
			if (file->name != NULL) {
				free(file->name);
			}
			if (file->path != NULL) {
				free(file->path);
			}

			free(file);
		}
	}
	free(leaf_name);

	return 0;
}
