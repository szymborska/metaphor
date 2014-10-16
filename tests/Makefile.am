ACLOCAL_AMFLAGS = -I m4

# Definitions
METAPHOR_LIBRARY_VERSION = 1:0:0
WARNINGS = -m64 -std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
noinst_bindir =  $(abs_top_builddir)
METAPHOR_DIRECTORY= @abs_top_builddir@
METAPHOR_TEMPLATE_DIRECTORY= @abs_top_builddir@/templates
METAPHOR_TESTING_LIBRARY_LOCATION=@abs_top_builddir@/tests/libmetaphortesting.so
METAPHOR_PLATFORM_TESTING_LIBRARY_LOCATION=@abs_top_builddir@/tests/libmetaphorplatformtesting.so
LOCAL_INCLUDE_DIRECTORY= -I@abs_top_builddir@/include
LOCAL_LIB_DIRECTORY= @abs_top_builddir@/libs
UNIX_PROGRAM_FLAGS=-DPS_EXISTS=$(PS_EXISTS) -DPS_PATH=\"$(PS_PATH)\" -DCAT_EXISTS=$(CAT_EXISTS) -DCAT_PATH=\"$(CAT_PATH)\" -DNE_EXISTS=$(NE_EXISTS)
noinst_libdir= @abs_top_builddir@/libs

noinst_bin_PROGRAMS = run_metaphor_filesystem_tests metaphor_filesystem_tests run_metaphor_processes_tests metaphor_processes_tests metaphor_unix_integration_tests hashutils_tests fileutils_tests stringutils_tests metaphor_memory_tests metaphor_memory_calls

hashutils_tests_SOURCES= hashutils_tests.c 
hashutils_tests_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd
hashutils_tests_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY)
 
fileutils_tests_SOURCES= fileutils_tests.c 
fileutils_tests_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd
fileutils_tests_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY)
  
stringutils_tests_SOURCES= stringutils_tests.c 
stringutils_tests_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd
stringutils_tests_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY)

metaphor_memory_tests_SOURCES= metaphor_memory_tests.c 
metaphor_memory_tests_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd
metaphor_memory_tests_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY)

metaphor_memory_calls_SOURCES= metaphor_memory_calls.c 
metaphor_memory_calls_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd
metaphor_memory_calls_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY)

run_metaphor_filesystem_tests_SOURCES= run_metaphor_filesystem_tests.c
run_metaphor_filesystem_tests_LDFLAGS = -lcmocka
run_metaphor_filesystem_tests_CFLAGS = -DMETAPHOR_LIBRARY_LOCATION=\"$(METAPHOR_TESTING_LIBRARY_LOCATION)\" 

metaphor_filesystem_tests_SOURCES= metaphor_filesystem_tests.c metaphor_filesystem_syscalls.c
metaphor_filesystem_tests_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd
metaphor_filesystem_tests_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY)
 
run_metaphor_processes_tests_SOURCES= run_metaphor_processes_tests.c
run_metaphor_processes_tests_LDFLAGS = -lcmocka
run_metaphor_processes_tests_CFLAGS = -DMETAPHOR_LIBRARY_LOCATION=\"$(METAPHOR_TESTING_LIBRARY_LOCATION)\" -DMETAPHOR_TEMPLATE_DIRECTORY=\"$(METAPHOR_TEMPLATE_DIRECTORY)\"

metaphor_processes_tests_SOURCES= metaphor_processes_tests.c
metaphor_processes_tests_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd
metaphor_processes_tests_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY) -DMETAPHOR_TEMPLATE_DIRECTORY=\"$(METAPHOR_TEMPLATE_DIRECTORY)\"

metaphor_unix_integration_tests_SOURCES= metaphor_unix_integration_tests.c
metaphor_unix_integration_tests_LDFLAGS = -lcmocka -L$(LOCAL_LIB_DIRECTORY) -lmetaphor -lbsd -ldl
metaphor_unix_integration_tests_CFLAGS = $(LOCAL_INCLUDE_DIRECTORY) -D_GNU_SOURCE -DMETAPHOR_TEMPLATE_DIRECTORY=\"$(METAPHOR_TEMPLATE_DIRECTORY)\" -DMETAPHOR_PLATFORM_TESTING_LIBRARY_LOCATION=\"$(METAPHOR_PLATFORM_TESTING_LIBRARY_LOCATION)\" -DMETAPHOR_DIRECTORY=\"$(METAPHOR_DIRECTORY)\" $(UNIX_PROGRAM_FLAGS) 

noinst_lib_LTLIBRARIES = libmetaphortesting.la libmetaphorplatformtesting.la 

# Since we use LD_PRELOAD, we have a separate library for our basic tests
libmetaphortesting_la_CFLAGS = -D_GNU_SOURCE $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY) -DMETAPHOR_DIRECTORY=\"$(METAPHOR_DIRECTORY)\" -DMETAPHOR_TEMPLATE_DIRECTORY=\"$(METAPHOR_TEMPLATE_DIRECTORY)\" 
libmetaphortesting_la_LIBADD = $(LOCAL_LIB_DIRECTORY)/libmetaphor.a -ldl
libmetaphortesting_la_SOURCES = metaphor_testing.c 
libmetaphortesting_la_LDFLAGS = -module -ldl -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd -no-undefined

# We also have a separate library that remaps system calls for integration tests
libmetaphorplatformtesting_la_CFLAGS = -D_GNU_SOURCE $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY) -DMETAPHOR_DIRECTORY=\"$(METAPHOR_DIRECTORY)\" -DMETAPHOR_TEMPLATE_DIRECTORY=\"$(METAPHOR_TEMPLATE_DIRECTORY)\" 
libmetaphorplatformtesting_la_LIBADD = $(LOCAL_LIB_DIRECTORY)/libmetaphor.a -ldl
libmetaphorplatformtesting_la_SOURCES = metaphor_platform_testing.c 
libmetaphorplatformtesting_la_LDFLAGS = -module -ldl -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd -no-undefined

# Copy libraries to the main directory
run_tests: run_metaphor_filesystem_tests metaphor_filesystem_tests run_metaphor_processes_tests metaphor_processes_tests metaphor_unix_integration_tests hashutils_tests fileutils_tests stringutils_tests metaphor_memory_tests metaphor_memory_calls libmetaphortesting.la libmetaphorplatformtesting.la 
	cp .libs/*.so . 
	./hashutils_tests 
	./stringutils_tests  
	./fileutils_tests 
	./run_metaphor_filesystem_tests
	./run_metaphor_processes_tests 
	./metaphor_unix_integration_tests

