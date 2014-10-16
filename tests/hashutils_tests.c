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

#include <metaphor/utils/hashutils.h>

static void
test_simple_dictionary(void **state)
{

	struct hash_entry *h;
	struct hash_entry *simple_dictionary = NULL;
	simple_dictionary = add_hash_int(simple_dictionary, "pid", 1002);
	simple_dictionary =
	    add_hash_string(simple_dictionary, "program", "process2");

	HASH_FIND_STR(simple_dictionary, "pid", h);
	char *pid_string = h->value;
	assert_string_equal(pid_string, "1002");

	HASH_FIND_STR(simple_dictionary, "program", h);
	char *program_string = h->value;
	assert_string_equal(program_string, "process2");

	free_dictionary(simple_dictionary);
}

int
main(void)
{
	const UnitTest tests[] = {
		unit_test(test_simple_dictionary),
	};
	return run_tests(tests);
}
