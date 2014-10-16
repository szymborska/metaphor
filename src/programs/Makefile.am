ACLOCAL_AMFLAGS = -I m4

# Definitions
METAPHOR_LIBRARY_VERSION = 1:0:0
INCLUDE_DIRECTORY= -I@abs_top_builddir@/include
PLATFORM_LOCATIONS=@abs_top_builddir@/configuration/locations.json
LOCAL_LIB_DIRECTORY= @abs_top_builddir@/libs
WARNINGS = -m64 -std=gnu99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
noinst_bindir =  $(abs_top_builddir)

bin_PROGRAMS = metaphor_install 
noinst_bin_PROGRAMS = metaphor metaphor_profile

metaphor_install_SOURCES= metaphor_app.c
metaphor_install_LDFLAGS = -ljansson -lbsd $(LOCAL_LIB_DIRECTORY)/libmetaphor.a
metaphor_install_CFLAGS = $(WARNINGS) $(INCLUDE_DIRECTORY) -DPLATFORM_LOCATIONS=\"$(datarootdir)/metaphor\"

metaphor_SOURCES= metaphor_app.c
metaphor_LDFLAGS = -ljansson -lbsd $(LOCAL_LIB_DIRECTORY)/libmetaphor.a
metaphor_CFLAGS = $(WARNINGS) $(INCLUDE_DIRECTORY) -DPLATFORM_LOCATIONS=\"$(PLATFORM_LOCATIONS)\"

metaphor_profile_SOURCES= metaphor_app.c
metaphor_profile_LDFLAGS = -ljansson -lbsd $(LOCAL_LIB_DIRECTORY)/libmetaphor.a
metaphor_profile_CFLAGS = $(WARNINGS) $(INCLUDE_DIRECTORY) -DPLATFORM_LOCATIONS=\"$(PLATFORM_LOCATIONS)\" -pg

all-local: 
	cp metaphor "@" 
	cp metaphor_profile $(noinst_bindir)
	cp "@" $(noinst_bindir)

clean-local: 
	rm -f "@" 

