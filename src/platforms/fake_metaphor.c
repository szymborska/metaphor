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

#include <metaphor/os/processes.h>
#include <metaphor/os/os.h>
#include <metaphor/os/filesystem.h>
#include <metaphor/os/whitelist.h>
#include <metaphor/utils/stringutils.h>

#include "fake_metaphor.h"

#pragma GCC diagnostic push	// require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

int initialize_metaphors(void);
int
initialize_metaphors(void)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();

	const char *data = "sample data\n";
	char *sample_data = safe_string_copy(256, (char *) data);

	new_file_with_static_data(filesystem, "/data/data.txt", sample_data,
			   strlen(sample_data));
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

