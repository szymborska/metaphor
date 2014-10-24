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

#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
// #include <fcntl.h>

#include <metaphor/os/filesystem.h>
#include <metaphor/os/processes.h>
#include <metaphor/os/os.h>
#include <metaphor/intercept/intercept_calls.h>

char * read_text_data(int blocking, int bytes_to_read, int read_offset);
char * read_text_data(int blocking, int bytes_to_read, int read_offset) {

   	FILE *file = fopen("./data/text.txt", "rb");

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

        if (read_offset + bytes_to_read > file_size) {
            bytes_to_read = file_size - read_offset;
        }
	fseek(file, read_offset, SEEK_SET);
	char *data = malloc(bytes_to_read + 1);
	int bytes_read = fread(data, 1, bytes_to_read, file);
 	if (bytes_read < bytes_to_read) {
		printf("Failed to read file. Only read %d bytes.\n", bytes_read);
		exit(1);
 	}
	fclose(file);
	data[bytes_read] = '\0';

	return data;
}

int size_text_data(char * throwaway_path);
int size_text_data(char * throwaway_path) {

   	FILE *file = fopen("./data/text.txt", "rb");
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);

        return file_size;
}
	
int initialize_metaphors(void);
int
initialize_metaphors(void)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();

	const char *data = "sample data\n";
	char *sample_data = malloc(sizeof (char) * strlen(data) + 1);
	strlcpy(sample_data, data, strlen(data) + 1);
	sample_data[strlen(data)] = '\0';

	new_file_with_static_data(filesystem, "/data/data.txt", sample_data,
			   strlen(sample_data));
	new_file_with_dynamic_data(filesystem, "/data/table.csv", &read_text_data, &size_text_data);

	char *whitelist_filepaths[] = { "/data/", "/proc", NULL };
	set_whitelist(whitelist_filepaths, 2);

	// Initialize the proc table
	initialize_proc();

	struct hash_entry *proc1_parameters = NULL;
	proc1_parameters = add_hash_int(proc1_parameters, "pid", 1001);
	proc1_parameters = add_hash_int(proc1_parameters, "time", 50);
	proc1_parameters =
	    add_hash_string(proc1_parameters, "program", "process1");

	/* Save the proc file */
	save_proc_file(proc1_parameters);

	/* Now, free the parameters that we passed in */
	free_dictionary(proc1_parameters);

	struct hash_entry *proc2_parameters = NULL;
	proc2_parameters = add_hash_int(proc2_parameters, "pid", 1002);
	proc2_parameters = add_hash_int(proc2_parameters, "time", 51);
	proc2_parameters =
	    add_hash_string(proc2_parameters, "program", "process2");

	/* Save the proc file */
	save_proc_file(proc2_parameters);

	/* Now, free the proc2_parameters that we passed in */
	free_dictionary(proc2_parameters);

	return 0;
}

SYSCALL_DEFINITIONS(initialize_metaphors)
