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

MYSQL *reusable_connection = NULL;
char * cached_username = NULL;
char * cached_password = NULL;

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
 	*username_length = 1024;
 	*password_length = 1024;
 
        if (cached_username == NULL || cached_password == NULL) {
    	    char * username = malloc(sizeof(char) * (*username_length));
 	    char * password = malloc(sizeof(char) * (*password_length));
     	    get_username_and_password(&username, username_length, &password, password_length); 
            cached_username = safe_string_copy(1024, username);
            cached_password = safe_string_copy(1024, password);
  	    free(username);
	    free(password);
        }

	if (mysql_real_connect
	    (connection, "localhost", cached_username, cached_password, "mysql", 0, NULL,
	     0) == NULL) {
		finish_with_error(connection);
	}

	free(username_length);
	free(password_length);

	return connection;
}

MYSQL *
get_mysql_connection(void);
MYSQL *
get_mysql_connection(void)
{
        if (!reusable_connection) { // Obviously, not thread safe
               reusable_connection = open_mysql_connection();
        } 
        return reusable_connection;
}

void 
close_mysql_connection(MYSQL * connection);
void 
close_mysql_connection(MYSQL * connection) {

	mysql_close(connection);
        reusable_connection = NULL;
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

	connection = get_mysql_connection();
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
}

char * read_table_data(struct m_file * table_file, int blocking, int bytes_to_read, int read_offset);
char * read_table_data(struct m_file * table_file, int blocking, int bytes_to_read, int read_offset) {

    printf("Inside read table data.\n");
    /* Get metadata */
    char * database_name = safe_string_copy(1024, (char *) table_file->metadata);
    char * table_name = safe_string_copy(1024, (char *) table_file->name);
    char * tmp_file_path = safe_join_strings(8095, 5, "/tmp/", database_name, ".", table_name, ".txt");

    printf("Trying to read file.\n");

    /* Dump the table */
    if( access( tmp_file_path, F_OK ) == -1 ) {
        MYSQL * connection = get_mysql_connection(); 

        char * dump_table_command = safe_join_strings(8096, 9, "SELECT * FROM ", database_name, ".", table_name, "INTO OUTFILE \"/tmp/", database_name, ".", table_name , ".txt\" FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n\'");
        printf("Dump table command: %s.\n", dump_table_command);
        make_query(connection, dump_table_command);
        free(dump_table_command);
    }

    /* Read the tmp file */
    FILE *file = fopen(tmp_file_path, "rb");

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

#if 0
    /* Get the table size */
    int table_size = get_table_size(m_file);
    table_file->length = table_size;

    /* Get table data */
    char * database_name = safe_string_copy(1024, (char *) table_file->metadata);
    char * table_name = safe_string_copy(1024, (char *) table_file->name);

    /* Estimate the row count needed */
    int row_count = 0;
    MYSQL_ROW row;
    unsigned int table_size = 0;
    while ((row = mysql_fetch_row(result))) { 
        row_count = safe_convert_string_to_int(strdup(row[0]));
    }
    int minimum_row_size = (int) ((float) table_size/ (float) row_count);
    int rows_to_read = bytes_to_read / minimum_row_size;

    /* Get the column names */
    const char * columns_query = safe_join_strings(1024, 5, "SELECT GROUP_CONCAT(COLUMN_NAME) FROM information_schema.COLUMNS WHERE table_schema = \"", database_name, "\" AND table_name = \"", table_name, "\"");
    result = make_query(connection, columns_query);
    MYSQL_ROW row;
    unsigned int table_size = 0;
    char * column_names;
    while ((row = mysql_fetch_row(result))) { 
        column_names = safe_string_copy(row[0]));
    }

    /* Get table data */
    char * database_name = safe_string_copy(1024, (char *) table_file->metadata);
    char * table_name = safe_string_copy(1024, (char *) table_file->name);
    const char * query = safe_join_strings(1024, 5, "SELECT CONCAT_WS(',',", column_names, ") FROM ", database_name, ".", table_name);
    result = make_query(connection, query);
#endif
    
}

int get_table_size(struct m_file * table_file);
int get_table_size(struct m_file * table_file) {

    MYSQL * connection = get_mysql_connection(); 
    MYSQL_RES *result; 

    char * database_name = safe_string_copy(1024, (char *) table_file->metadata);
    char * table_name = safe_string_copy(1024, (char *) table_file->name);
    const char * query = safe_join_strings(1024, 5, "SELECT data_length + index_length  FROM information_schema.TABLES  WHERE table_schema = \"", database_name, "\" AND table_name = \"", table_name, "\"");
    result = make_query(connection, query);

    MYSQL_ROW row;
    unsigned int table_size = 0;
    while ((row = mysql_fetch_row(result))) { 
        table_size = safe_convert_string_to_int(strdup(row[0]));
    }

    mysql_free_result(result);

    return table_size;
}

void create_table_files_for_database(MYSQL * connection, char * database_name);
void create_table_files_for_database(MYSQL * connection, char * database_name) {

    MYSQL_RES *result; 

    const char * query = safe_join_strings(1024, 2, "show tables from ", database_name);
    result = make_query(connection, query);

    MYSQL_ROW row;
    struct m_file * filesystem = get_root_filesystem();
    while ((row = mysql_fetch_row(result))) { 
        
        /* Basic table information */
        char * table = strdup(row[0]);
        char * file_name = safe_join_strings(8096, 4, "/data/csv/", database_name, "/", table);
        char * copy_of_database_name = safe_string_copy(8096, database_name);

        /* Create a dynamic table file */ 
        new_file_with_dynamic_data(filesystem, file_name, (uintptr_t) copy_of_database_name, &read_table_data, &get_table_size, NULL);
        
        free(file_name);
        free(table);
    }

    mysql_free_result(result);
}

void create_table_files(void);
void create_table_files(void) {

    MYSQL *connection;
    MYSQL_RES *result; 

    connection = get_mysql_connection();
    const char * query = "show databases";
    result = make_query(connection, query);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) { 
        char * database = strdup(row[0]);
        create_table_files_for_database(connection, database);
        free(database);
    }
                    
    mysql_free_result(result);
}

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
	char *whitelist_filepaths[] = { "/data", "/proc", NULL };
	set_whitelist(whitelist_filepaths, 2);

	// Initialize the proc table
	initialize_proc();

	// Give it a try
	create_query_processes();

        // Create the table files
        create_table_files();
 
        // Close the MySQL connection
        MYSQL * connection = get_mysql_connection();
        close_mysql_connection(connection);

	return 0;
}
