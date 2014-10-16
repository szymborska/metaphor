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

int
main(int argc, char **argv)
{

	int return_code = 0;
	const char *metaphor_library_path = METAPHOR_LIBRARY_LOCATION;
	unsigned int preload_length =
	    strlen("LD_PRELOAD=") + strlen(metaphor_library_path) + strlen(" ");
	const char *program_name = "./metaphor_filesystem_tests";
	unsigned int program_command_length =
	    preload_length + strlen(program_name);
	char *program_command =
	    malloc(sizeof (char) * program_command_length + 1);
	strcpy(program_command, "LD_PRELOAD=");
	strcat(program_command, metaphor_library_path);
	strcat(program_command, " ");
	strcat(program_command, program_name);
	program_command[program_command_length] = '\0';

	return_code = system(program_command);

	return return_code;
}
