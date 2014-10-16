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
#include <bsd/string.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <wchar.h>

#include <metaphor/utils/stringutils.h>

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

int ESTRINGTRUNCATED = 15485863;	// 1 millionth prime to avoid collision
int ESTRINGCONVERSION = 15485864;

/*
 * All of the string functions below are intentionally "grumpy" at this
 * point in the project and **set errno** upon failure or truncation.
 */

char *
safe_convert_int_to_string(int value)
{

	char *number_string = malloc(sizeof (char) * 20);
	int return_val = snprintf(number_string, 20, "%d", value);
	if (return_val < 0) {
		perror
		    ("In convert int to string: snprintf returned an error.\n");
		errno = ESTRINGCONVERSION;
	}

	return number_string;
}

int
safe_convert_string_to_int(char *value)
{

	char *leftover = NULL;
	int original_errno = errno;
	long int_value = strtol(value, &leftover, 10);

	if ((!original_errno) && (errno != 0)) {
		perror
		    ("In convert_string_to_int: strtol - failed to convert string to an integer.\n");
		printf("String is %s.\n", value);
		errno = ESTRINGCONVERSION;
	}

	return (int) int_value;
}

char *
safe_string_copy(int max_size, char *string)
{

	int minimum_size = MIN(max_size, strlen(string) + 1);
	char *return_string = malloc(sizeof (char) * minimum_size);
	strlcpy(return_string, string, minimum_size);

	if (minimum_size < strlen(string)) {
		errno = ESTRINGTRUNCATED;
	}

	return return_string;
}

char *
safe_substring_copy(int max_size, char *string, int beginning, int end)
{

	// Add one character to get the full substring (i.e., 4 - 2
	// results in 2 but there are three characters from 
	// position 2 to 4) and one character for null character
	int string_size = end - beginning + 2;
	int minimum_size = MIN(max_size, string_size);
	char *return_string = malloc(sizeof (char) * string_size);
	strlcpy(return_string, (string + beginning), string_size);

	if (minimum_size < string_size) {
		errno = ESTRINGTRUNCATED;
	}

	return return_string;
}

// Create one string from several strings. Note, there
// is the potential for mischief here since there is a
// dependency on count being correct. 
char *
safe_join_strings(int max_size, int count, ...)
{

	// Find the size of all of the strings 
	va_list string_list;
	va_start(string_list, count);
	unsigned int string_size = 0;
	unsigned int string_index;
	for (string_index = 0; string_index < count; string_index++) {
		char *argument_string = va_arg(string_list, char *);
		string_size += strlen(argument_string);

	}
	va_end(string_list);

	// Make our string be the *minimum* size
	int minimum_size = MIN(max_size, string_size + 1);
	char *return_string = malloc(sizeof (char) * minimum_size);

	// Now, walk through and append copies of the string
	va_start(string_list, count);
	int first_string = 1;
	for (string_index = 0; string_index < count; string_index++) {
		char *argument_string = va_arg(string_list, char *);
		if (first_string) {
			strlcpy(return_string, argument_string, minimum_size);
		} else {
			strlcat(return_string, argument_string, minimum_size);
		}
		first_string = 0;
	}
	va_end(string_list);

	if (minimum_size < string_size) {
		errno = ESTRINGTRUNCATED;
	}

	return return_string;
}
