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
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <metaphor/utils/stringutils.h>

static void
test_conversion_functions(void **state)
{

	char *int_string = safe_convert_int_to_string(10010);
	int int_from_string = safe_convert_string_to_int("99999");

	assert_string_equal(int_string, "10010");
	assert_int_equal(int_from_string, 99999);
	free(int_string);
}

static void
test_safe_string_copy(void **state)
{

	char *hello_world_string = safe_string_copy(100, "hello world");
	assert_int_equal(0, errno);
	char *truncated_string = safe_string_copy(5, "hello world");
	assert_int_equal(ESTRINGTRUNCATED, errno);
	errno = 0;

	assert_string_equal("hello world", hello_world_string);
	free(hello_world_string);
	free(truncated_string);
}

static void
test_safe_substring_copy(void **state)
{

	char *hello_world_string =
	    safe_substring_copy(100, "hello world", 1, 5);
	assert_int_equal(0, errno);
	char *truncated_string = safe_substring_copy(5, "hello world", 0, 9);
	assert_int_equal(ESTRINGTRUNCATED, errno);
	errno = 0;

	assert_string_equal("ello ", hello_world_string);
	free(hello_world_string);
	free(truncated_string);
}

static void
test_safe_join_strings(void **state)
{

	char *hello_world_string =
	    safe_join_strings(100, 3, "hello world", "beep", "hello");
	assert_int_equal(0, errno);
	char *truncated_string =
	    safe_join_strings(5, 2, "hello world", "goodbye");
	assert_int_equal(ESTRINGTRUNCATED, errno);
	errno = 0;

	assert_string_equal("hello worldbeephello", hello_world_string);
	free(hello_world_string);
	free(truncated_string);
}

static void
test_unicode(void **state) {

	char * filepath = "快乐/财富/财富";
	char *copy_string = safe_string_copy(100, filepath);
	assert_int_equal(0, errno);
	assert_string_equal(filepath,copy_string);
	free(copy_string);

	char * join_strings = safe_join_strings(100, 3, "快", "乐", "/财富");
	assert_int_equal(0, errno);
	assert_string_equal(join_strings, "快乐/财富");
	free(join_strings);

	char *path_pointer = strchr(filepath, '/');
	char *subpath;
	if (path_pointer != NULL) {
		path_pointer = path_pointer + 1;
		int distance = (int) (path_pointer - filepath);
		if (distance > 0) {
			subpath = safe_substring_copy(8096, filepath, 0, distance - 2);
		}
	}
	assert_int_equal(0, errno);
	assert_string_equal(subpath, "快乐");
	free(subpath);
}

int
main(void)
{
	const UnitTest tests[] = {
		unit_test(test_conversion_functions),
		unit_test(test_safe_string_copy),
		unit_test(test_safe_substring_copy),
		unit_test(test_safe_join_strings),
		unit_test(test_unicode),
	};
	return run_tests(tests);
}
