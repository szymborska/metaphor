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
#include <stdlib.h>
#include <unistd.h>
#include <bsd/string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dlfcn.h>

#include "metaphor/utils/debug.h"
#include "metaphor/utils/stringutils.h"

unsigned int read_debugging_environment_variables = 0;	// only read env once
unsigned int debugging_flag = 0;	// 0 no debug, !0 debug
char *debugging_filename = NULL;	// if set, debug redirected to filename
pthread_mutex_t debug_initialized_mutex;

void
read_debugging_environment(void)
{

	pthread_mutex_lock(&debug_initialized_mutex);
	if (!read_debugging_environment_variables) {
		char *main_debug_string = getenv("METAPHOR_DEBUG");
		if (main_debug_string != NULL) {
			debugging_flag =
			    safe_convert_string_to_int(main_debug_string);

			char *filename = getenv("METAPHOR_FILENAME");
			if (filename != NULL) {
				unsigned int filename_length =
				    sizeof (char) * strlen(filename);
				debugging_filename =
				    malloc(filename_length + 1);
				strlcpy(debugging_filename, filename,
					filename_length + 1);
				mode_t mode = S_IRWXU | S_IRWXG;
				int (*syscall_unlink) (const char *pathname);
				*(void **) &syscall_unlink =
				    dlsym(RTLD_NEXT, "unlink");
				syscall_unlink(get_debug_filename());
				int (*syscall_open) (const char *filename,
						     int flags, mode_t mode);
				*(void **) &syscall_open =
				    dlsym(RTLD_NEXT, "open");
				int (*syscall_chmod) (const char *path,
						      mode_t mode);
				*(void **) &syscall_chmod =
				    dlsym(RTLD_NEXT, "chmod");
				unsigned int fd =
				    syscall_open(get_debug_filename(),
						 O_WRONLY | O_CREAT, mode);
				syscall_chmod(get_debug_filename(), mode);
				int (*syscall_close) (int fd);
				*(void **) &syscall_close =
				    dlsym(RTLD_NEXT, "close");
				syscall_close(fd);
			}
		}
	}

	read_debugging_environment_variables = 1;
	pthread_mutex_unlock(&debug_initialized_mutex);
}

unsigned int
get_debugging_flag(void)
{
	return debugging_flag;
}

char *
get_debug_filename(void)
{
	return debugging_filename;
}
