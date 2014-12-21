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
#include <metaphor/os/os.h>
#include <metaphor/os/whitelist.h>
#include <metaphor/intercept/syscall.h>
#include <metaphor/utils/stringutils.h>
#include <metaphor/utils/debug.h>

#pragma GCC diagnostic push	// require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

int
m_unlink(const char *pathname)
{

	int return_code = -1;
	int (*syscall_unlink) (const char *pathname);
	*(void **) &syscall_unlink = dlsym(RTLD_NEXT, "unlink");

	if (!check_whitelist(pathname) || get_initializing_resources()) {
		debug_print("%s: Using original syscall.\n", "M_UNLINK");
		return_code = syscall_unlink(pathname);
	} else {
		struct m_file *file_to_delete =
		    get_file(get_root_filesystem(), (char *) pathname);
		return_code = m_delete(get_root_filesystem(), file_to_delete);
	}

	return return_code;
}

int
m_open(const char *pathname, int flags)
{

	int return_fd = -1;
	int (*syscall_open) (const char *filename, int flags);
	*(void **) &syscall_open = dlsym(RTLD_NEXT, "open");

	if (!check_whitelist(pathname) || get_initializing_resources()) {
		debug_print("%s: Using original syscall.\n", "M_OPEN");
		return_fd = syscall_open(pathname, flags);
	} else {
		debug_print("M_OPEN: Trying to get file: %s.\n", pathname);
		struct m_file *file =
		    get_file(get_root_filesystem(), (char *) pathname);
		struct m_open_file *opened_file =
		    malloc(sizeof (struct m_open_file));
		debug_print("M_OPEN: Opening %s.\n", pathname);
		if (file == NULL) {
			debug_print("%s: OPEN FILE IS NULL.\n", "M_OPEN");
			return_fd = -1;
			errno = EACCES;
		} else {
			opened_file->file = file;
			opened_file->position = 0;
                        opened_file->read_offset = 0;
			unsigned int current_fd = get_current_fd();
			current_fd = current_fd + 1;
			return_fd = current_fd;
			set_current_fd(current_fd);
			opened_file->fd = return_fd;
			opened_file->blocking = 0;
			debug_print("Adding the fd: %d.\n", return_fd);

			struct m_open_file *open_files = get_open_files();
			HASH_ADD_INT(open_files, fd, opened_file);
			set_open_files(open_files);
		}
	}

	return return_fd;
}

ssize_t
m_read(int fd, void *buf, size_t count)
{

	debug_print("Inside m_read trying to read %d bytes.\n", (int) count);
	ssize_t read_bytes = 0;
	struct m_open_file *open_file;
	struct m_file *file;
	debug_print("The fd is: %d.\n", fd);
	struct m_open_file *open_files = get_open_files();
	HASH_FIND_INT(open_files, &fd, open_file);
	set_open_files(open_files);

	ssize_t(*syscall_read) (int fd, void *buf, size_t count);
	*(void **) &syscall_read = dlsym(RTLD_NEXT, "read");

	if (open_file && !get_initializing_resources()) {
		debug_print("The open file fd is %d.\n", open_file->fd);
		debug_print("The open file position is %d.\n",
			    open_file->position);
		debug_print("%s: Past looking at paths.\n", "M_READ");
		debug_print("%s.\n", open_file->file->path);

		if (!check_whitelist(open_file->file->path)) {
			debug_print
			    ("%s Using original syscall, not in whitelist.\n",
			     "M_READ");
			read_bytes = syscall_read(fd, buf, count);
		} else {
			file = open_file->file;
                        if (file->read_function != NULL) {
				if (!open_file->eof_reached) { 
 
					char * data = (*file->read_function)(file,0,count,open_file->read_offset);
                                        int bytes_read = strlen(data);
					memcpy(buf, data, bytes_read);
	
                	                open_file->read_offset += bytes_read;
					open_file->position += bytes_read; 
					read_bytes = bytes_read;
					if (strlen(data) < count) {
						open_file->eof_reached = 1; 
                                        }
                                } else {
                                        read_bytes = 0;
                                }

                        } else {

				if ((file->length - open_file->position) > count) {
					debug_print("The data is: %s.\n",
					    (char *) file->data);
					debug_print("The position is: %d.\n",
					    open_file->position);
					char *offset =
					    (char *) file->data + open_file->position;
					debug_print("The offset data is: %s.\n",
					    (char *) offset);
					memcpy(buf, offset, count);
					char *char_buffer = (char *) buf;
					char_buffer[count] = '\0';
					open_file->position += count;
					debug_print("FULLY copied: %s.\n",
					    (char *) buf);
					read_bytes = count;
				} else if (file->length == open_file->position) {
					debug_print("%s. Returning NULL.\n", "EQUAL");
					debug_print("File length: %d.\n", file->length);
					debug_print("Position: %d.\n",
					    open_file->position);

					read_bytes = 0;
					// char * char_buffer = (char *) buf;
					// char_buffer[0] = '\0';

				} else {
					// We don't handle blocking: if (open_file->blocking)
					debug_print("%s. NOT equal.\n", "NOT EQUAL");
					int remaining_bytes =
					    file->length - open_file->position;
					debug_print("File length: %d.\n", file->length);
					debug_print("Position: %d.\n",
					    open_file->position);
					char *offset =
					    (char *) file->data + open_file->position;

					debug_print
					    ("We are trying to copy back data: %s.\n",
					     (char *) file->data);
					memcpy(buf, offset, remaining_bytes);
					debug_print("Offset: %s.\n", offset);
					debug_print("Size: %d.\n", remaining_bytes);
					// buf[remaining_bytes] = '\0';
					char *char_buffer = (char *) buf;
					char_buffer[remaining_bytes] = '\0';
					debug_print("What we actually copied: %s.\n",
					    (char *) buf);
					open_file->position += remaining_bytes;
					read_bytes = remaining_bytes;
				}
			}
		}
	} else {
		debug_print("%s Using original syscall, not in whitelist.\n",
			    "M_READ");
		read_bytes = syscall_read(fd, buf, count);
		debug_print("%s Called the syscall.\n", "M_READ");
	}
	debug_print("M_READ: Returning with %d bytes.\n", (int) read_bytes);

	return read_bytes;
}

int
m_close(int fd)
{

	int return_code = 0;
	struct m_open_file *open_file;
	struct m_open_file *open_files = get_open_files();
	HASH_FIND_INT(open_files, &fd, open_file);
	int (*syscall_close) (int fd);
	*(void **) &syscall_close = dlsym(RTLD_NEXT, "close");

	if (open_file && !get_initializing_resources()) {
		if (!check_whitelist(open_file->file->path)) {
			return_code = syscall_close(fd);
		} else {
			// open_files = get_open_files();
			HASH_DEL(open_files, open_file);
			set_open_files(open_files);
			free(open_file);
			return_code = 0;
		}
	} else {
		return_code = syscall_close(fd);
	}

	return return_code;
}

off_t
m_lseek(int fd, off_t offset, int whence)
{

	int return_code = 0;
	struct m_open_file *open_file;
	struct m_open_file *open_files = get_open_files();
	HASH_FIND_INT(open_files, &fd, open_file);
	set_open_files(open_files);
	off_t(*syscall_lseek) (int fd, off_t offset, int whence);
	*(void **) &syscall_lseek = dlsym(RTLD_NEXT, "lseek");

	if (open_file && !get_initializing_resources()) {
		if (!check_whitelist(open_file->file->path)) {
			return_code = syscall_lseek(fd, offset, whence);
		} else {
			if (whence == SEEK_SET) {
				open_file->position = offset;
				open_file->read_offset = offset;
			} else if (whence == SEEK_CUR) {
				open_file->position += offset;
				open_file->read_offset += offset;
			} else if (whence == SEEK_END) {
				open_file->position =
				    open_file->file->length + offset;
				open_file->read_offset =
				    open_file->file->length + offset;
			}
			return_code = open_file->position;
		}
	} else {
		return_code = syscall_lseek(fd, offset, whence);
	}

	return return_code;
}

DIR *m_opendir(const char *pathname);
DIR *
m_opendir(const char *pathname)
{
	debug_print("%s Inside function.\n", "M_OPENDIR");
	DIR *directory_pointer;
	DIR *(*syscall_opendir) (const char *pathname);
	*(void **) &syscall_opendir = dlsym(RTLD_NEXT, "opendir");

	if (!check_whitelist(pathname) || get_initializing_resources()) {
		debug_print
		    ("OPEN DIR: Using original syscall, %s not in whitelist.\n",
		     pathname);
		directory_pointer = (DIR *) syscall_opendir(pathname);
	} else {
		debug_print("M_OPENDIR: Trying to get file: %s.\n", pathname);
		struct m_file *directory_file =
		    get_file(get_root_filesystem(), (char *) pathname);
		if (directory_file == NULL) {
			debug_print("%s: DIRECTORY FILE IS NULL.\n",
				    "M_OPENDIR");
		}
		if (directory_file) {
			long unsigned int current_directory_pointer_count =
			    get_currect_directory_pointer_count() + 1;
			set_current_directory_pointer_count
			    (current_directory_pointer_count);

			directory_pointer =
			    (DIR *) current_directory_pointer_count;
			struct m_directory *current_directory =
			    malloc(sizeof (struct m_directory));
			current_directory->directory_pointer =
			    (long unsigned int) directory_pointer;
			current_directory->pathname =
			    safe_string_copy(8096, (char *) pathname);
			current_directory->file = directory_file;

			struct m_file *entry = NULL;
			struct m_file *tmp = NULL;
			struct dirent *last_dirent = NULL;
			int first_entry = 1;

			HASH_ITER(hh, directory_file->files, entry, tmp) {

				struct dirent *current_dirent =
				    malloc(sizeof (struct dirent));
				if (first_entry) {
					current_directory->dirent_entries =
					    current_dirent;
					current_directory->current_dirent =
					    current_dirent;
					first_entry = 0;
				}
				// We don't want to double allocate so just a strlcpy for
				// now. 
				strlcpy(current_dirent->d_name, entry->name,
					strlen(entry->name) + 1);
				current_dirent->d_ino = entry->inode;
				current_dirent->d_reclen =
				    sizeof (struct dirent);
				current_dirent->d_off = (off_t) NULL;
				current_dirent->d_type = 0;
				if (entry->is_directory) {
					current_dirent->d_type =
					    current_dirent->d_type & DT_DIR;
				} else {
					current_dirent->d_type =
					    current_dirent->d_type & DT_REG;
				}
				if (last_dirent != NULL) {
					last_dirent->d_off =
					    (off_t) current_dirent;
				}
				last_dirent = current_dirent;
			}

			struct m_directory *open_directories =
			    get_open_directories();
			HASH_ADD_INT(open_directories, directory_pointer,
				     current_directory);
			set_open_directories(open_directories);
		} else {
			directory_pointer = NULL;
			errno = EACCES;
		}
	}

	return directory_pointer;
}

struct dirent *m_readdir(DIR * dirp);
struct dirent *
m_readdir(DIR * dirp)
{

	struct dirent *current_dirent;
	long unsigned int cast_directory_pointer = (long unsigned int) dirp;
	struct dirent *(*syscall_readdir) (DIR * dirp);
	*(void **) &syscall_readdir = dlsym(RTLD_NEXT, "readdir");
	struct m_directory *current_directory;

	struct m_directory *open_directories = get_open_directories();
	HASH_FIND_INT(open_directories, &cast_directory_pointer,
		      current_directory);
	if (current_directory && !get_initializing_resources()) {
		if (!check_whitelist(current_directory->pathname)) {
			current_dirent = syscall_readdir(dirp);
		} else {
			current_dirent =
			    (struct dirent *) current_directory->current_dirent;
			if (current_dirent != NULL) {
				current_directory->current_dirent =
				    (struct dirent *) current_directory->
				    current_dirent->d_off;
			}
		}
	} else {
		current_dirent = syscall_readdir(dirp);
	}

	return current_dirent;
}

int m_closedir(DIR * dirp);
int
m_closedir(DIR * dirp)
{

	int return_code = 0;
	long unsigned int cast_directory_pointer = (long unsigned int) dirp;
	int (*syscall_closedir) (DIR * dirp);
	*(void **) &syscall_closedir = dlsym(RTLD_NEXT, "closedir");
	struct m_directory *current_directory;

	struct m_directory *open_directories = get_open_directories();
	HASH_FIND_INT(open_directories, &cast_directory_pointer,
		      current_directory);

	if (current_directory && !get_initializing_resources()) {
		if (!check_whitelist(current_directory->pathname)) {
			return_code = syscall_closedir(dirp);
		} else {
			open_directories = get_open_directories();
			HASH_DEL(open_directories, current_directory);
			set_open_directories(open_directories);
			free(current_directory->pathname);
			free(current_directory);
			return_code = 0;
		}
	} else {
		return_code = syscall_closedir(dirp);
	}

	return return_code;
}

int m_xstat(int format, const char *path, struct stat *buf);
int
m_xstat(int format, const char *path, struct stat *buf)
{

	int return_code = 0;
	int (*syscall_xstat) (int format, const char *path, struct stat * buf);
	*(void **) &syscall_xstat = dlsym(RTLD_NEXT, "__xstat");

	if (!check_whitelist(path) || get_initializing_resources()) {
		return_code = syscall_xstat(format, path, buf);
	} else {
		debug_print("M_XSTAT: Trying to get file %s.\n", path);
		struct m_file *file =
		    get_file(get_root_filesystem(), (char *) path);
		if (file) {

			/* A lot of this is arbitrary nonsense. */
			buf->st_dev = 1001;
			buf->st_ino = file->inode;
			if (file->is_directory) {
				buf->st_mode = S_IFDIR;
			} else {
				buf->st_mode = S_IFREG;
			}
			buf->st_nlink = 1;	// made up
			buf->st_uid = 0;	// made up
			buf->st_gid = 0;	// made up
                        if (file->size_function != NULL) {
                             buf->st_size = (off_t) (*(file->size_function))(file);
                        } else {
   			     buf->st_size = (long long) file->length;
                        }

			buf->st_blksize = (long) 4096;	// made up
			buf->st_blocks = (long long) 24;	// made up
		} else {
			file = NULL;
			errno = EACCES;
		}
	}

	return return_code;
}

int m_lxstat(int format, const char *path, struct stat *buf);
int
m_lxstat(int format, const char *path, struct stat *buf)
{

	// Technically we should call the syscall lstat but are
	// calling xstat in the function
	return m_xstat(format, path, buf);
}

int m_fxstat(int format, int fd, struct stat *buf);
int
m_fxstat(int format, int fd, struct stat *buf)
{

	int return_code = 0;
	struct m_open_file *open_file;
	struct m_open_file *open_files = get_open_files();
	HASH_FIND_INT(open_files, &fd, open_file);
	set_open_files(open_files);
	int (*syscall_fxstat) (int format, int fd, struct stat * buf);
	*(void **) &syscall_fxstat = dlsym(RTLD_NEXT, "__fxstat");

	if (open_file && !get_initializing_resources()) {
		if (!check_whitelist(open_file->file->path)) {
			return_code = syscall_fxstat(format, fd, buf);
		} else {
			return_code =
			    m_xstat(format, open_file->file->path, buf);
		}
	} else {
		return_code = syscall_fxstat(format, fd, buf);
	}

	return return_code;
}

int m_openat(int dirfd, const char *pathname, int flags);
int
m_openat(int dirfd, const char *pathname, int flags)
{

	int fd = -1;
	int (*syscall_openat) (int dirfd, const char *pathname, int flags);
	*(void **) &syscall_openat = dlsym(RTLD_NEXT, "openat");
	struct m_open_file *open_file;
	struct m_open_file *open_files = get_open_files();
	HASH_FIND_INT(open_files, &dirfd, open_file);
	set_open_files(open_files);

	if (open_file && !get_initializing_resources()) {
		if (!check_whitelist(open_file->file->path)) {
			fd = syscall_openat(dirfd, pathname, flags);
		} else {
			char *full_path =
			    safe_join_strings(8096, 3, open_file->file->path,
					      "/", pathname);
			fd = m_open(full_path, flags);
		}
	} else {

		fd = syscall_openat(dirfd, pathname, flags);
	}

	return fd;
}

int
m_pause(void)
{

	return 1000;
}
