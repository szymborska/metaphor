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
#include <jansson.h>
#include <stdlib.h>
#include <unistd.h>
#include <bsd/string.h>

#include <metaphor/utils/stringutils.h>

/* Utility function from jansson library */
static char *loadfile(FILE * file);
static char *
loadfile(FILE * file)
{
	long fsize, ret;
	char *buf;

	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	buf = malloc(fsize + 1);
	ret = fread(buf, 1, fsize, file);
	if (ret != fsize)
		exit(1);
	buf[fsize] = '\0';

	return buf;
}

char *load_configuration(const char *configuration_path,
			 const char *program_name);
char *
load_configuration(const char *configuration_path, const char *program_name)
{

	/* Load the configuration file. */
	char *found_path = NULL;
	json_t *root;
	json_error_t error;

	FILE *configuration_file;
	if (!(configuration_file = fopen(configuration_path, "rb"))) {
		fprintf(stderr, "Could not open \"%s\"\n", configuration_path);
		exit(2);
	}

	char *text = loadfile(configuration_file);
	root = json_loads(text, 0, &error);
	free(text);
        fclose(configuration_file);

	if (!root) {
		fprintf(stderr, "error: on line %d: %s\n", error.line,
			error.text);
		exit(1);
	}

	if (!json_is_array(root)) {
		fprintf(stderr, "error: root is not an array\n");
		json_decref(root);
		exit(1);
	}

	int i;
	for (i = 0; i < json_array_size(root); i++) {

		json_t *data, *retrieved_program_name, *path;

		data = json_array_get(root, i);
		if (!json_is_object(data)) {
			fprintf(stderr,
				"error: commit data %d is not an object\n",
				i + 1);
			json_decref(root);
			exit(1);
		}

		retrieved_program_name = json_object_get(data, "name");
		if (!json_is_string(retrieved_program_name)) {
			fprintf(stderr,
				"error: commit %d: name is not a string\n",
				i + 1);
			json_decref(root);
			exit(1);
		}

		path = json_object_get(data, "path");
		if (!json_is_string(path)) {
			fprintf(stderr,
				"error: commit %d: path is not a string\n",
				i + 1);
			json_decref(root);
			exit(1);
		}

		if (!strcmp
		    (program_name, json_string_value(retrieved_program_name))) {
			found_path =
			    strndup(json_string_value(path),
				    strlen(json_string_value(path)));
		}
	}

	return found_path;
}

char *get_metaphor_program_name(char **argv);
char *
get_metaphor_program_name(char **argv)
{

	char *metaphor_program_name;
	char *metaphor_argument = argv[1];
	char *path_pointer = strrchr(metaphor_argument, '/');
	if (path_pointer != NULL) {
		path_pointer = path_pointer + 1;
		int distance = (int) (path_pointer - metaphor_argument) + 1;
		if (distance > 0) {
			metaphor_program_name =
			    safe_string_copy(8096, path_pointer);
		} else {
			metaphor_program_name =
			    safe_string_copy(8096, metaphor_argument);
		}
	} else {
		metaphor_program_name =
		    safe_string_copy(8096, metaphor_argument);
	}

	return metaphor_program_name;
}

char *get_command_arguments(int argc, char **argv);
char *
get_command_arguments(int argc, char **argv)
{

	unsigned int string_length = 0;
	int i;
	for (i = 3; i < argc; i++) {
		string_length += strlen(argv[i]);
		string_length += 1;	// for the space
	}

	char *command_arguments = malloc(sizeof (char) * string_length + 1);
	if (argc > 3) {
		strlcpy(command_arguments, argv[3], string_length + 1);	// copies "one" into str_output
		for (i = 4; i < argc; i++) {
			strlcat(command_arguments, " ", string_length + 1);
			strlcat(command_arguments, argv[i], string_length + 1);	//  attaches str_two to str_output
		}
	}

	return command_arguments;
}

int run_program(const char *metaphor_library_path, const char *program_name,
		const char *command_arguments);
int
run_program(const char *metaphor_library_path, const char *program_name,
	    const char *command_arguments)
{

	char *program_command =
	    safe_join_strings(8096, 6, "LD_PRELOAD=", metaphor_library_path,
			      " ", program_name, " ", command_arguments);
	int return_code = system(program_command);
	free(program_command);

	return return_code;
}

int
main(int argc, char **argv)
{

	int return_code = 0;
	if (argc >= 3) {

		const char *configuration_path = PLATFORM_LOCATIONS;
		char *metaphor_program_name = get_metaphor_program_name(argv);
		char *metaphor_library_path =
		    load_configuration(configuration_path,
				       metaphor_program_name);

		if (metaphor_library_path != NULL) {
			char *hosted_program_name = argv[2];
			char *command_arguments =
			    get_command_arguments(argc, argv);

			return_code =
			    run_program(metaphor_library_path,
					hosted_program_name, command_arguments);
			free(metaphor_library_path);
			free(command_arguments);
		} else {
			fprintf(stderr,
				"Metaphor support for %s is not installed.\n",
				metaphor_program_name);
			return_code = -1;
		}
	}

	return return_code;
}
