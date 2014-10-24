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
#include <bsd/string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <bsd/string.h>

#include <mysql.h>

#include <metaphor/third-party/uthash.h>
#include <metaphor/utils/hashutils.h>
#include <metaphor/utils/stringutils.h>
#include <metaphor/os/whitelist.h>
#include <metaphor/os/filesystem.h>
#include <metaphor/os/processes.h>
#include <metaphor/os/authentication.h>
#include <metaphor/os/os.h>
#include <metaphor/intercept/intercept_calls.h>

#include "mysql_metaphor.h"

#pragma GCC diagnostic push	// require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

void finish_with_error(MYSQL * connection);
void
finish_with_error(MYSQL * connection)
{

	fprintf(stderr, "%s\n", mysql_error(connection));
	mysql_close(connection);
}

MYSQL *open_mysql_connection(void);
MYSQL *
open_mysql_connection(void)
{

	MYSQL *connection = mysql_init(NULL);

	if (connection == NULL) {
		fprintf(stderr, "mysql_init() failed\n");
		exit(1);
	}

 	int * username_length = malloc(sizeof(int));
 	int * password_length = malloc(sizeof(int));
 	*username_length = 1000;
 	*password_length = 1000;
 	char * username = malloc(sizeof(char) * (*username_length));
 	char * password = malloc(sizeof(char) * (*password_length));

 	get_username_and_password(&username, username_length, &password, password_length); 

	if (mysql_real_connect
	    (connection, "localhost", username, password, "mysql", 0, NULL,
	     0) == NULL) {
		finish_with_error(connection);
	}

	free(username_length);
	free(username);
	free(password_length);
	free(password);

	return connection;
}

int make_query_without_result(MYSQL * connection, const char *query);
int
make_query_without_result(MYSQL * connection, const char *query)
{

	int error_occurred = 0;
	if (mysql_query(connection, query)) {
		finish_with_error(connection);
		error_occurred = 1;
	}

	return error_occurred;
}

MYSQL_RES *make_query(MYSQL * connection, const char *query);
MYSQL_RES *
make_query(MYSQL * connection, const char *query)
{

	MYSQL_RES *result;

	if (mysql_query(connection, query)) {
		finish_with_error(connection);
	}

	result = mysql_store_result(connection);
	if (result == NULL) {
		finish_with_error(connection);
	}

	return result;
}

int save_mysql_proc_file(int mysql_pid, int time, char *program_name);
int
save_mysql_proc_file(int mysql_pid, int time, char *program_name)
{

	struct hash_entry *parameters = NULL;
	parameters = add_hash_int(parameters, "pid", mysql_pid);
	parameters = add_hash_int(parameters, "time", time);
	parameters = add_hash_string(parameters, "program", program_name);

	/* Save the proc file */
	save_proc_file(parameters);

	/* Now, free the parameters that we passed in */
	free_dictionary(parameters);

	return 0;
}

void create_query_processes(void);
void
create_query_processes(void)
{

	MYSQL *connection;
	MYSQL_RES *result;

	connection = open_mysql_connection();
	const char *query = "show processlist";
	result = make_query(connection, query);

	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
		char *pid_string = strdup(row[0]);
		char *program;
		if ((row[num_fields - 1]) != NULL) {
			program = safe_string_copy(1024, row[num_fields - 1]);
		} else {
			program = safe_string_copy(1024, row[4]);
		}

		char *leftover = NULL;
		long mysql_pid = strtol(pid_string, &leftover, 10);
		if ((errno != 0 && mysql_pid == 0)) {
			perror("strtol");
			exit(EXIT_FAILURE);
		}
		save_mysql_proc_file((int) mysql_pid, 0, program);
		if (row[num_fields - 1] != NULL) {
			free(program);
		}
		free(pid_string);
	}

	mysql_free_result(result);
	mysql_close(connection);
}

/*
void create_database_directories(void) {

    MYSQL *connection;
    MYSQL_RES *result; 

    connection = open_mysql_connection();
    const char * query = "show databases";
    result = make_query(connection, query);

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) { 
        char * database = strdup(row[0]);
        create_partial_database_tables(database);
        free(database);
    }
                    
    mysql_free_result(result);
    mysql_close(connection);
}
*/

int kill(pid_t pid, int sig);
int
kill(pid_t pid, int sig)
{

	MYSQL *connection;
	int error_occurred;

	char kill_command[50];
	snprintf(kill_command, 50, "kill \"%d\"", (int) pid);

	connection = open_mysql_connection();
	error_occurred = make_query_without_result(connection, kill_command);
	if (!error_occurred) {
		mysql_close(connection);
	}

	return 0;
}

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

	// Give it a try
	create_query_processes();

	return 0;
}
