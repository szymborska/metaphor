ACLOCAL_AMFLAGS = -I m4

# Definitions
METAPHOR_LIBRARY_VERSION = 1:0:0
WARNINGS = -m64 -std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
LOCAL_INCLUDE_DIRECTORY= -I@abs_top_builddir@/include
LOCAL_LIB_DIRECTORY= @abs_top_builddir@/libs
noinst_bindir =  $(abs_top_builddir)
noinst_libdir= @abs_top_builddir@/libs

lib_LTLIBRARIES = libmysql_metaphor.la libfake_metaphor.la 

noinst_lib_LTLIBRARIES = libmysql_metaphor_profile.la libfake_metaphor_profile.la

# The metaphor mysql library
libmysql_metaphor_la_CFLAGS = -D_GNU_SOURCE `mysql_config --cflags` $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY) 
libmysql_metaphor_la_SOURCES = mysql_metaphor.c mysql_syscalls.c
libmysql_metaphor_la_LIBADD = $(LOCAL_LIB_DIRECTORY)/libmetaphor.a -ldl -lbsd
libmysql_metaphor_la_LDFLAGS = -module -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd `mysql_config --libs` $(LOCAL_LIB_DIRECTORY)

# A profiling version of the metaphor mysql library
libmysql_metaphor_profile_la_CFLAGS = -D_GNU_SOURCE `mysql_config --cflags` $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY) -pg
libmysql_metaphor_profile_la_SOURCES = mysql_metaphor.c mysql_syscalls.c
libmysql_metaphor_profile_la_LIBADD = $(LOCAL_LIB_DIRECTORY)/libmetaphor.a -ldl -lbsd
libmysql_metaphor_profile_la_LDFLAGS = -module -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd `mysql_config --libs` $(LOCAL_LIB_DIRECTORY)

# The fake metaphor library
libfake_metaphor_la_CFLAGS = -D_GNU_SOURCE $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY)
libfake_metaphor_la_SOURCES = fake_metaphor.c fake_metaphor_syscalls.c
libfake_metaphor_la_LIBADD = $(LOCAL_LIB_DIRECTORY)/libmetaphor.a -ldl -lbsd
libfake_metaphor_la_LDFLAGS = -module -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd $(LOCAL_LIB_DIRECTORY) -no-undefined

# A profiling version of the fake metaphor library
libfake_metaphor_profile_la_CFLAGS = -D_GNU_SOURCE $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY) -pg
libfake_metaphor_profile_la_SOURCES = fake_metaphor.c fake_metaphor_syscalls.c
libfake_metaphor_profile_la_LIBADD = $(LOCAL_LIB_DIRECTORY)/libmetaphor.a -ldl -lbsd
libfake_metaphor_profile_la_LDFLAGS = -module -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd $(LOCAL_LIB_DIRECTORY) -no-undefined

# Copy libraries to the main directory
all-local: libmysql_metaphor.la 
	cp .libs/*.so . 
	cp .libs/* $(LOCAL_LIB_DIRECTORY) 

