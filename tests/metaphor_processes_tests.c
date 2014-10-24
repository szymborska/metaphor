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

#include <metaphor/os/filesystem.h>
#include <metaphor/os/processes.h>
#include <metaphor/utils/fileutils.h>
#include "metaphor_processes_tests.h"

int
initialize_metaphors(void)
{
}

static void
test_initialize_proc(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);
	struct m_file *entry;
	struct m_file *tmp;

	initialize_proc();

	char *uptime_data = "2362430.60 4644862.92";
	char *data = malloc(sizeof (char) * strlen(uptime_data) + 1);

	int fd = open("/proc/uptime", O_RDONLY);
	int bytes_read = read(fd, data, strlen(uptime_data));
	data[bytes_read] = '\0';
	close(fd);

	assert_string_equal(data, uptime_data);
	delete_filesystem(filesystem);
}

static void
test_create_processes(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);
	struct m_file *entry;
	struct m_file *tmp;

	initialize_proc();

	struct hash_entry *parameters = NULL;
	parameters = add_hash_int(parameters, "pid", 1001);
	parameters = add_hash_int(parameters, "time", 50);
	parameters = add_hash_string(parameters, "program", "process1");

	/* Save the proc file */
	save_proc_file(parameters);

	/* Now, free the parameters that we passed in */
	free_dictionary(parameters);

	char *stat_data =
	    "1001 (process1) S 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 5000 0 0 18446744073709551615 0 0 0 0 0 0 0 2147483647 0 18446744073709551615 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
	char *data = malloc(sizeof (char) * strlen(stat_data) + 1);

	int fd = open("/proc/1001/stat", O_RDONLY);
	int bytes_read = read(fd, data, strlen(stat_data));
	data[bytes_read] = '\0';
	close(fd);

	assert_string_equal(data, stat_data);

	delete_filesystem(filesystem);
}

int
main(void)
{
	const UnitTest tests[] = {
		unit_test(test_initialize_proc),
		unit_test(test_create_processes),
	};
	return run_tests(tests);
}
