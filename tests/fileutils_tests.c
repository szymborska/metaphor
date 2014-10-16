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
#include <stdlib.h>

#include <metaphor/utils/fileutils.h>

static void
test_read_nonbinary_file(void **state)
{
	char *expected_data = "Hello World! - Just a text file\n";
	char *file_data = read_nonbinary_file("./data/hello.txt");
	assert_string_equal(expected_data, file_data);

	free(file_data);
}

static void
test_unicode(void **state) {
	char * filepath = "快乐/财富/财富";
	char * leaf = get_leaf(filepath) ;
	char * subpath = get_next_sub_path(filepath);
        assert_string_equal(leaf, "财富");
        assert_string_equal(subpath, "快乐");
}

int
main(void)
{
	const UnitTest tests[] = {
		unit_test(test_read_nonbinary_file),
		unit_test(test_unicode),
	};
	return run_tests(tests);
}
