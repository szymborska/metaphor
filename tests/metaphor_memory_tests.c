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

#include <metaphor/utils/stringutils.h>

static void
run_memory_tests(void **state)
{

	int program_index;
	int result;
	int number_of_functions;

	char *program_name = "./metaphor_memory_calls";
	char *argument = safe_convert_int_to_string(-1);
	char *valgrind_command =
	    "valgrind --quiet --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --error-exitcode=1 ./metaphor_memory_calls";

	argument = safe_convert_int_to_string(-1);
	char *program_command =
	    safe_join_strings(8096, 3, program_name, " ", argument);
	number_of_functions = system(program_command);
	number_of_functions = number_of_functions >> 8;
	free(program_command);
	free(argument);

	for (program_index = 0; program_index < number_of_functions;
	     program_index++) {
		argument = safe_convert_int_to_string(program_index);
		program_command =
		    safe_join_strings(8096, 3, valgrind_command, " ", argument);
		result = system(program_command);
		result = result >> 8;
		assert_int_equal(result, 0);	// metaphor_memory_calls always return 0 on
		// function calls. valgrind returns 1 when
		// there is a leak
		free(argument);
		free(program_command);
	}
}

int
main(void)
{
	const UnitTest tests[] = {
		unit_test(run_memory_tests),
	};
	return run_tests(tests);
}
