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

#include <metaphor/os/filesystem.h>
#include "metaphor_filesystem_tests.h"

int
initialize_metaphors(void)
{
}

static void
test_pause_syscall(void **state)
{
	int result = pause();
	assert_int_equal(1000, result);
}

static void
test_new_file_with_parameters(void **state)
{
	int result = pause();
	char *name = "sample.txt";
	char *data = "data";
	struct m_file *file =
	    new_file_with_parameters(name, name, (uintptr_t) NULL, data, strlen(data), 0, 1, 0, NULL, NULL, NULL);

	assert_int_equal(0, file->permissions);
	assert_int_equal(1, file->is_directory);
	assert_int_equal(0, file->is_root);
	assert_string_equal(data, (char *) file->data);
	assert_string_equal(name, file->name);

	free(file);
}

static void
test_new_filesystem(void **state)
{

	struct m_file *filesystem = new_filesystem();

	assert_int_equal(0, filesystem->permissions);
	assert_int_equal(1, filesystem->is_directory);
	assert_int_equal(1, filesystem->is_root);
	assert_int_equal(NULL, filesystem->data);
	assert_string_equal("/", filesystem->name);

	delete_filesystem(filesystem);
}

static void
test_single_root_file(void **state)
{

	struct m_file *filesystem = new_filesystem();
	struct m_file *file = new_file(filesystem, "/beep");
	struct m_file *returned_file = get_file(filesystem, "/beep");

	assert_string_equal(file->name, returned_file->name);

	delete_filesystem(filesystem);
}

static void
test_multiple_root_files(void **state)
{

	struct m_file *filesystem = new_filesystem();
	struct m_file *beep_file = new_file(filesystem, "/beep");
	struct m_file *test_file = new_file(filesystem, "/test.txt");
	struct m_file *bop_file = new_file(filesystem, "/bop");

	struct m_file *returned_file = get_file(filesystem, "/beep");
	assert_string_equal(beep_file->name, returned_file->name);

	returned_file = get_file(filesystem, "/test.txt");
	assert_string_equal(test_file->name, returned_file->name);

	returned_file = get_file(filesystem, "/bop");
	assert_string_equal(bop_file->name, returned_file->name);

	delete_filesystem(filesystem);
}

static void
test_nested_files(void **state)
{

	struct m_file *filesystem = new_filesystem();
	struct m_file *entry;
	struct m_file *tmp;

	struct m_file *stat1_file = new_file(filesystem, "/proc/1/stat");
	struct m_file *stat2_file = new_file(filesystem, "/proc/2/stat");
	struct m_file *passwd_file = new_file(filesystem, "/etc/passwd");

	struct m_file *returned_file = get_file(filesystem, "/proc/1/stat");
	assert_string_equal(stat1_file->name, returned_file->name);

	returned_file = get_file(filesystem, "/proc/2/stat");
	assert_string_equal(stat2_file->name, returned_file->name);

	returned_file = get_file(filesystem, "/etc/passwd");
	assert_string_equal(passwd_file->name, returned_file->name);

	delete_filesystem(filesystem);
}

static void
test_nested_files_with_data(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	struct m_file *entry;
	struct m_file *tmp;

	const char *data1 = "stat 1";
	char *stat_data1 = malloc(sizeof (char) * strlen(data1) + 1);
	strlcpy(stat_data1, data1, sizeof (stat_data1));

	const char *data2 = "stat 1";
	char *stat_data2 = malloc(sizeof (char) * strlen(data2) + 1);
	strlcpy(stat_data2, data2, sizeof (stat_data2));

	const char *data3 = "passwd";
	char *passwd_data = malloc(sizeof (char) * strlen(data3) + 1);
	strlcpy(passwd_data, data3, sizeof (passwd_data));

	struct m_file *stat1_file =
	    new_file_with_static_data(filesystem, "/proc/1/stat", stat_data1,
			       strlen(stat_data1));
	struct m_file *stat2_file =
	    new_file_with_static_data(filesystem, "/proc/2/stat", stat_data2,
			       strlen(stat_data2));
	struct m_file *passwd_file =
	    new_file_with_static_data(filesystem, "/etc/passwd", passwd_data,
			       strlen(passwd_data));

	struct m_file *returned_file = get_file(filesystem, "/proc/1/stat");
	assert_string_equal(stat1_file->name, returned_file->name);
	assert_string_equal((char *) stat1_file->data,
			    (char *) returned_file->data);

	returned_file = get_file(filesystem, "/proc/2/stat");
	assert_string_equal(stat2_file->name, returned_file->name);
	assert_string_equal((char *) stat2_file->data,
			    (char *) returned_file->data);

	returned_file = get_file(filesystem, "/etc/passwd");
	assert_string_equal(passwd_file->name, returned_file->name);
	assert_string_equal((char *) passwd_file->data,
			    (char *) returned_file->data);

	delete_filesystem(filesystem);
}

static void
test_open_file(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	struct m_file *entry;
	struct m_file *tmp;

	const char *data1 = "stat 1";
	char *stat_data1 = malloc(sizeof (char) * strlen(data1) + 1);
	strlcpy(stat_data1, data1, sizeof (stat_data1));

	struct m_file *stat1_file =
	    new_file_with_static_data(filesystem, "/proc/1/stat", stat_data1,
			       strlen(stat_data1));
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);

	int fd = open("/proc/1/stat", O_RDONLY);
	close(fd);
	assert_int_equal(fd, 100001);
	delete_filesystem(filesystem);
}

static void
test_read_file_exactly(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	struct m_file *entry;
	struct m_file *tmp;

	const char *data1 = "stat 1";
	char *stat_data1 = malloc(sizeof (char) * strlen(data1) + 1);
	strlcpy(stat_data1, data1, sizeof (stat_data1));

	struct m_file *stat1_file =
	    new_file_with_static_data(filesystem, "/proc/1/stat", stat_data1,
			       strlen(stat_data1));
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);

	int fd = open("/proc/1/stat", O_RDONLY);
	char buffer[5];
	int bytes_read = read(fd, buffer, 4);
	buffer[bytes_read] = '\0';
	close(fd);
	assert_int_equal(fd, 100001);
	delete_filesystem(filesystem);
}

static void
test_read_beyond_file(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	struct m_file *entry;
	struct m_file *tmp;

	const char *data1 = "stat 1";
	char *stat_data1 = malloc(sizeof (char) * strlen(data1) + 1);
	strlcpy(stat_data1, data1, sizeof (stat_data1));

	struct m_file *stat1_file =
	    new_file_with_static_data(filesystem, "/proc/1/stat", stat_data1,
			       strlen(stat_data1));
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);

	int fd = open("/proc/1/stat", O_RDONLY);
	char buffer[8];
	int bytes_read = read(fd, buffer, 8);
	buffer[bytes_read] = '\0';
	close(fd);
	assert_int_equal(fd, 100001);
	delete_filesystem(filesystem);
}

char * read_text_data(struct m_file * file, int blocking, int bytes_to_read, int read_offset) {

   	FILE *text_file = fopen("./data/text.txt", "rb");

	fseek(text_file, 0, SEEK_END);
	long file_size = ftell(text_file);
	fseek(text_file, 0, SEEK_SET);

        if (read_offset + bytes_to_read > file_size) {
            bytes_to_read = file_size - read_offset;
        }
	fseek(text_file, read_offset, SEEK_SET);
	char *data = malloc(bytes_to_read + 1);
	int bytes_read = fread(data, 1, bytes_to_read, text_file);
 	if (bytes_read < bytes_to_read) {
		printf("Failed to read file. Only read %d bytes.\n", bytes_read);
		exit(1);
 	}
	fclose(text_file);
	data[bytes_read] = '\0';

	return data;
}

int size_text_data(struct m_file * file) {

   	FILE *file_ptr = fopen("./data/text.txt", "rb");
	fseek(file_ptr, 0, SEEK_END);
	long file_size = ftell(file_ptr);

        return file_size;
}
	
static void
test_new_file_with_dynamic_data(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	
	char *whitelist_filepaths[] = { "/data", NULL };
	set_whitelist(whitelist_filepaths, 1);

	struct m_file *file = new_file_with_dynamic_data(filesystem, "/data/table.csv", (uintptr_t) NULL, &read_text_data, &size_text_data, NULL);
        
	int fd = m_open("/data/table.csv", O_RDONLY);

	char buffer1[8];
	int bytes_read = m_read(fd, buffer1, 8);
	buffer1[bytes_read] = '\0';
	assert_string_equal(buffer1, "The Saw-");

	char buffer2[8];
	bytes_read = m_read(fd, buffer2, 8);
	buffer2[bytes_read] = '\0';
	assert_string_equal(buffer2, "Horse at");

	m_close(fd);

        // Stat the file and get the length
        struct stat buf;
        m_xstat(0, "/data/table.csv", &buf);
        assert_int_equal((int) buf.st_size, 463);

	delete_filesystem(filesystem);
}

static void
test_stat_file(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	struct m_file *entry;
	struct m_file *tmp;

	const char *data1 = "stat 1";
	char *stat_data1 = malloc(sizeof (char) * strlen(data1) + 1);
	strlcpy(stat_data1, data1, sizeof (stat_data1));

	struct m_file *stat1_file =
	    new_file_with_static_data(filesystem, "/proc/1/stat", stat_data1,
			       strlen(stat_data1));
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);

	struct stat file_stat;
	if (stat("/proc/1/stat", &file_stat) != -1) {
		if (file_stat.st_mode == S_IFDIR) {
			assert_true(0);
		} else if (file_stat.st_mode == S_IFREG) {
			assert_true(1);
		}
	} else {
		assert_true(1);
	}

	delete_filesystem(filesystem);
}

static void
test_openat(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	struct m_file *entry;
	struct m_file *tmp;

	const char *data1 = "stat 1";
	char *stat_data1 = malloc(sizeof (char) * strlen(data1) + 1);
	strlcpy(stat_data1, data1, sizeof (stat_data1));

	struct m_file *stat1_file =
	    new_file_with_static_data(filesystem, "/proc/1/stat", stat_data1,
			       strlen(stat_data1));
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);

	int dirfd = open("/proc/1", O_RDONLY);
	int fd = openat(dirfd, "stat", O_RDONLY);
	assert_int_equal(fd, 100002);

	char buffer[8];
	int bytes_read = read(fd, buffer, 8);
	buffer[bytes_read] = '\0';
	close(fd);

	assert_string_equal(buffer, "stat 1");

	delete_filesystem(filesystem);
}

static void
test_reading_a_directory(void **state)
{

	initialize_filesystem();
	struct m_file *filesystem = get_root_filesystem();
	char *whitelist_filepaths[] = { "/proc", NULL };
	set_whitelist(whitelist_filepaths, 1);
	struct m_file *entry;
	struct m_file *tmp;

	const char *data1 = "stat 1";
	char *proc_data1 = malloc(sizeof (char) * strlen(data1) + 1);
	strlcpy(proc_data1, data1, sizeof (proc_data1));

	const char *data2 = "stat 2";
	char *proc_data2 = malloc(sizeof (char) * strlen(data2) + 1);
	strlcpy(proc_data2, data2, sizeof (proc_data2));

	const char *data3 = "stat 3";
	char *proc_data3 = malloc(sizeof (char) * strlen(data3) + 1);
	strlcpy(proc_data3, data3, sizeof (proc_data3));

	struct m_file *proc1_file =
	    new_file_with_static_data(filesystem, "/proc/1/stat", proc_data1,
			       strlen(proc_data1));
	struct m_file *proc2_file =
	    new_file_with_static_data(filesystem, "/proc/1/status", proc_data2,
			       strlen(proc_data2));
	struct m_file *proc3_file =
	    new_file_with_static_data(filesystem, "/proc/1/io", proc_data3,
			       strlen(proc_data3));

	int succeeded = 0;
	struct dirent *dp;
	DIR *dir;

	int saw_stat = 0;
	int saw_status = 0;
	int saw_io = 0;
	if ((dir = opendir("/proc/1")) != NULL) {
		while ((dp = readdir(dir)) != NULL) {
			if (strcmp(dp->d_name, ".") == 0
			    || strcmp(dp->d_name, "..") == 0) {
				continue;
			}
			if (!strcmp(dp->d_name, "stat")) {
				saw_stat = 1;
			}

			if (!strcmp(dp->d_name, "status")) {
				saw_status = 1;
			}

			if (!strcmp(dp->d_name, "io")) {
				saw_io = 1;
			}
		}
		closedir(dir);
	}

	assert_int_equal(1, saw_stat);
	assert_int_equal(1, saw_status);
	assert_int_equal(1, saw_io);

	delete_filesystem(filesystem);
}

int
main(void)
{
	const UnitTest tests[] = {
		unit_test(test_pause_syscall),
		unit_test(test_new_file_with_parameters),
		unit_test(test_new_filesystem),
		unit_test(test_single_root_file),
		unit_test(test_multiple_root_files),
		unit_test(test_nested_files),
		unit_test(test_nested_files_with_data),
		unit_test(test_open_file),
		unit_test(test_read_file_exactly),
		unit_test(test_read_beyond_file),
		unit_test(test_reading_a_directory),
		unit_test(test_new_file_with_dynamic_data),
		unit_test(test_stat_file),
		unit_test(test_openat),
	};
	return run_tests(tests);
}
