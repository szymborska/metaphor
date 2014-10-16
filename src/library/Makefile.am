ACLOCAL_AMFLAGS = -I m4

# Definitions
METAPHOR_LIBRARY_VERSION = 1:0:0
WARNINGS = -m64 -std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
METAPHOR_DIRECTORY= @abs_top_builddir@
METAPHOR_TEMPLATE_DIRECTORY= @abs_top_builddir@/templates
LOCAL_INCLUDE_DIRECTORY= -I@abs_top_builddir@/include
LOCAL_LIB_DIRECTORY= @abs_top_builddir@/libs
PLATFORMS_DIRECTORY= @abs_top_builddir@/platforms
CORE_FILES= debug.c authentication.c hashutils.c stringutils.c fileutils.c whitelist.c filesystem.c processes.c os.c syscall.c 
noinst_bindir=  $(abs_top_builddir)
noinst_libdir= @abs_top_builddir@/libs

lib_LTLIBRARIES = libmetaphor.la 

noinst_lib_LTLIBRARIES = libmetaphor_profile.la

# The metaphor library
libmetaphor_la_CFLAGS = -D_GNU_SOURCE $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY) -DMETAPHOR_DIRECTORY=\"$(METAPHOR_DIRECTORY)\" -DMETAPHOR_TEMPLATE_DIRECTORY=\"$(METAPHOR_TEMPLATE_DIRECTORY)\" -fPIC
libmetaphor_la_SOURCES = $(CORE_FILES) 
libmetaphor_la_LDFLAGS = -module -ldl -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd

# The metaphor library (with profiling)
libmetaphor_profile_la_CFLAGS = -D_GNU_SOURCE $(WARNINGS) $(LOCAL_INCLUDE_DIRECTORY) -DMETAPHOR_DIRECTORY=\"$(METAPHOR_DIRECTORY)\" -DMETAPHOR_TEMPLATE_DIRECTORY=\"$(METAPHOR_TEMPLATE_DIRECTORY)\" -fPIC -pg
libmetaphor_profile_la_SOURCES = $(CORE_FILES) 
libmetaphor_profile_la_LDFLAGS = -module -ldl -version-info $(METAPHOR_LIBRARY_VERSION) -D_GNU_SOURCE -lpthread -lbsd

# Copy the library to the main directory so that platform libraries can link to it
all-local: libmetaphor.la libmetaphor_profile.la
	mkdir -p $(LOCAL_LIB_DIRECTORY) 
	cp .libs/* $(LOCAL_LIB_DIRECTORY) 

