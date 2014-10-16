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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <metaphor/utils/stringutils.h>

const int number_of_functions = 1;

int
run_string_operations()
{

	char *int_string = safe_convert_int_to_string(10010);
	int int_from_string = safe_convert_string_to_int("99999");
	char *hello_world_string1 = safe_string_copy(100, "hello world");
	char *hello_world_string2 =
	    safe_substring_copy(100, "hello world", 1, 5);
	char *hello_world_string3 =
	    safe_join_strings(100, 3, "hello world", "beep", "hello");
	free(hello_world_string1);
	free(hello_world_string2);
	free(hello_world_string3);
	free(int_string);

	return 0;
}

int
main(int argc, char **argv)
{

	int return_code = 0;

	if (argc == 2) {

		char *argument = argv[1];
		int function_switch = safe_convert_string_to_int(argument);
		if (function_switch == -1) {	// special case, give number of method calls
			return_code = number_of_functions;
		} else {
			switch (function_switch) {
			case 0:
				run_string_operations();
				break;
			default:
				return_code = -1;
				break;
			}
		}
	}

	return return_code;
}
