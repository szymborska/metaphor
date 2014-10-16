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
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ftw.h>
#include <time.h>

#include "metaphor/third-party/uthash.h"
#include "metaphor/utils/stringutils.h"
#include "metaphor/utils/fileutils.h"
#include "metaphor/utils/debug.h"
#include "metaphor/os/processes.h"
#include "metaphor/os/filesystem.h"

void save_proc_file_internal(int pid, int time_in_seconds, char *program);
void
save_proc_file(struct hash_entry *dictionary)
{

	struct hash_entry *h;

	HASH_FIND_STR(dictionary, "pid", h);
	char *pid_string = h->value;

	HASH_FIND_STR(dictionary, "time", h);
	char *time_string = h->value;

	HASH_FIND_STR(dictionary, "program", h);
	char *program = h->value;

	int pid = safe_convert_string_to_int(pid_string);
	int time = safe_convert_string_to_int(time_string);

	save_proc_file_internal(pid, time, program);
}

static long int get_hertz(void);
static long int
get_hertz(void)
{
	return sysconf(_SC_CLK_TCK);
}

static long int seconds2jiffies(int seconds);
static long int
seconds2jiffies(int seconds)
{
	long int hertz = get_hertz();
	long int jiffies = seconds * hertz;

	return jiffies;
}

void save_stat_file(char *stat_file_path, int pid, int time_in_seconds,
		    char *program);
void
save_stat_file(char *stat_file_path, int pid, int time_in_seconds,
	       char *program)
{

	struct m_file *filesystem = get_root_filesystem();

	char *stat_string = malloc(sizeof (char) * 1000 + 1);
	long int jiffies = seconds2jiffies(time_in_seconds);
	char *pid_string = safe_convert_int_to_string(pid);

	int return_value =
	    snprintf(stat_string, 1000,
		     "%d (%s) S 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 %ld 0 0 18446744073709551615 0 0 0 0 0 0 0 2147483647 0 18446744073709551615 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0",
		     pid, program, jiffies);
	if (return_value < 0) {
		perror("save_stat_file: failed calling snprintf.\n");
		exit(EXIT_FAILURE);
	}
	debug_print("Stat string: %s.\n", stat_string);

	// Save to our root filesystem here
	// struct m_file * stat_file = 
	new_file_with_data(filesystem, stat_file_path, stat_string,
			   strlen(stat_string));
	free(pid_string);
}

void
save_proc_file_internal(int pid, int time_in_seconds, char *program)
{

	struct m_file *filesystem = get_root_filesystem();
	char *pid_string = safe_convert_int_to_string(pid);
	char *proc_directory = "/proc";

	char *proc_pid_directory =
	    safe_join_strings(8096, 3, proc_directory, "/", pid_string);
	debug_print("Proc pid directory: %s.\n", proc_pid_directory);

	char *stat_file_to =
	    safe_join_strings(8096, 3, proc_pid_directory, "/", "stat");
	debug_print("Stat file to: %s.\n", stat_file_to);

	struct m_file *file_to_delete =
	    get_file(filesystem, proc_pid_directory);

	// Check return code here
	if (file_to_delete != NULL) {
		m_delete(filesystem, file_to_delete);
	}

	save_stat_file(stat_file_to, pid, time_in_seconds, program);
	free(pid_string);
	free(proc_pid_directory);
	free(stat_file_to);
}

void copy_templates(void);
void
copy_templates(void)
{

	char *proc_directory = "/proc";
	char **t, *templates[] =
	    { "stat", "meminfo", "vmstat", "uptime", NULL };
	struct m_file *filesystem = get_root_filesystem();

	t = templates;
	while (*t != NULL) {

		char *copy_to =
		    safe_join_strings(8096, 3, proc_directory, "/", *t);
		char *metaphor_template_directory = METAPHOR_TEMPLATE_DIRECTORY;
		char *copy_from =
		    safe_join_strings(8096, 3, metaphor_template_directory, "/",
				      *t);

		debug_print("Copying %s to %s.\n", copy_from, copy_to);
		char *file_data = read_nonbinary_file(copy_from);
		debug_print("File data: %s.\n", file_data);
		new_file_with_data(filesystem, copy_to, file_data,
				   strlen(file_data));
		free(copy_to);
		free(copy_from);
		t++;
	}
}

void
initialize_proc(void)
{

	char *proc_directory = "/proc";
	struct m_file *filesystem = get_root_filesystem();
	pid_t pid = getpid();
	char *pid_string = safe_convert_int_to_string(pid);

	// Delete and recreate the proc directory 
	struct m_file *file_to_delete = get_file(filesystem, proc_directory);
	// Look at the return code: int return_code = 
	if (file_to_delete != NULL) {
		m_delete(filesystem, file_to_delete);
	}
	// NOTE: We're skipping permissions here again
	// mkdir(proc_directory, 0777);

	char *proc_pid_directory =
	    safe_join_strings(8096, 3, proc_directory, "/", pid_string);
	debug_print("Proc pid directory: %s.\n", proc_pid_directory);

	char *self_proc_directory =
	    safe_join_strings(8096, 3, proc_directory, "/", "self");
	debug_print("Self proc directory: %s.\n", self_proc_directory);

	char *self_proc_stat =
	    safe_join_strings(8096, 3, self_proc_directory, "/", "stat");
	debug_print("Self proc stat: %s.\n", self_proc_stat);

	file_to_delete = get_file(filesystem, proc_directory);
	// Return code should be checked: return_code = 
	if (file_to_delete != NULL) {
		m_delete(filesystem, file_to_delete);
	}

	save_proc_file_internal((int) pid, 0, "init");
	debug_print("Self proc stat path: %s.\n", self_proc_stat);
	save_stat_file(self_proc_stat, pid, 0, "init");

	copy_templates();
}
