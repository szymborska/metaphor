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
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>

#include <metaphor/os/filesystem.h>

char *
run_program(char *program_path, char *program_arguments)
{

	int return_code = 0;

	const char *metaphor_platform_testing_library_path =
	    METAPHOR_PLATFORM_TESTING_LIBRARY_LOCATION;
	unsigned int preload_length =
	    strlen("LD_PRELOAD=") +
	    strlen(metaphor_platform_testing_library_path) + strlen(" ") +
	    strlen(program_path) + strlen(" ") + strlen(program_arguments);
	char *preload_command = malloc(sizeof (char) * preload_length + 1);
	FILE *p_file;
	int data_size = 4096;
	char *data = malloc(sizeof (char) * data_size + 1);

	snprintf(preload_command, preload_length + 1, "LD_PRELOAD=%s %s %s",
		 metaphor_platform_testing_library_path, program_path,
		 program_arguments);
	preload_command[preload_length] = '\0';

	p_file = popen(preload_command, "r");
	if (!p_file) {
		// fail with no output
		printf("Failed to popen!\n");
		data[0] = '\0';
	} else {
		int p_file_no = fileno(p_file);
		ssize_t(*syscall_read) (int fd, void *buf, size_t count);
		*(void **) &syscall_read = dlsym(RTLD_NEXT, "read");
		syscall_read(p_file_no, data, data_size);

		if (pclose(p_file) != 0) {
			// fail with no output
			data[0] = '\0';
		}
	}

	return data;
}

#ifdef CAT_EXISTS
static void
test_run_cat(void **state)
{
	int cat_succeeded = 0;
	char *data = run_program(CAT_PATH, "/data/data.txt");
	if (strstr(data, "sample data") != NULL) {
		cat_succeeded = 1;
	}

	free(data);
	assert_int_equal(1, cat_succeeded);
}
#endif

#ifdef PS_EXISTS
static void
test_run_ps(void **state)
{

	int ps_succeeded = 0;
	char *data = run_program(PS_PATH, "aux");

	if (strstr(data, "process1") != NULL) {
		ps_succeeded = 1;
	}

	free(data);
	assert_int_equal(1, ps_succeeded);
}
#endif

int
main(void)
{
	const UnitTest tests[] = {
#if CAT_EXISTS
		unit_test(test_run_cat),
#endif
#if PS_EXISTS
		unit_test(test_run_ps),
#endif
#if NE_EXISTS
		intentional syntax error	// fail to compile if non_existant found
#endif
	};
	return run_tests(tests);
}
