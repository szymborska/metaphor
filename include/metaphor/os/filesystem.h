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

#ifndef METAPHOR_FILESYSTEM_H
#define METAPHOR_FILESYSTEM_H

#include <metaphor/os/types.h>
#include <metaphor/third-party/uthash.h>

typedef char *(*READ_DATA) (int blocking, int bytes_to_read);

struct m_file {
	char *name;
	char *path;
	void *data;
	int inode;
	int length;
	int is_directory;
	int permissions;
	int is_root;
	struct m_file *files;
	UT_hash_handle hh;
};

struct m_open_file {
	int fd;
	struct m_file *file;
	int position;
	int blocking;
	READ_DATA *read_data;
	UT_hash_handle hh;
};

struct m_directory {
	long unsigned int directory_pointer;
	char *pathname;
	struct m_file *file;
	struct dirent *dirent_entries;
	struct dirent *current_dirent;
	UT_hash_handle hh;
};

int initialize_filesystem(void);
void set_root_filesystem(struct m_file *file);
long unsigned int get_currect_directory_pointer_count(void);
void set_current_directory_pointer_count(long unsigned int new_count);
unsigned int get_current_fd(void);
void set_current_fd(unsigned int new_fd);
struct m_file *get_root_filesystem(void);
struct m_open_file *get_open_files(void);
void set_open_files(struct m_open_file *new_open_files);
struct m_directory *get_open_directories(void);
void set_open_directories(struct m_directory *new_open_directories);
struct m_file *new_file_with_parameters(char *name, char *path, void *data,
					int length, int permissions,
					int is_directory, int is_root);
struct m_file *new_file(struct m_file *filesystem, char *path);
struct m_file *new_file_with_data(struct m_file *filesystem, char *path,
				  void *data, int length);
struct m_file *get_file(struct m_file *filesystem, char *path);
struct m_file *new_filesystem(void);
void delete_filesystem(struct m_file *filesystem);
struct m_file *get_directory_of_file(struct m_file *filesystem, char *path,
				     int should_create_directories);
int m_delete(struct m_file *filesystem, struct m_file *file_to_delete);

#endif
