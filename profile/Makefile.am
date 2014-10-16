ACLOCAL_AMFLAGS = -I m4

# Definitions
METAPHOR_LIBRARY_VERSION = 1:0:0
WARNINGS = -m64 -std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
noinst_bindir =  $(abs_top_builddir)

all-local: 
	$(noinst_bindir)/src/programs/metaphor_profile fake_profile $(PS_PATH) 
	mv gmon.out ps.gmon.out
	gprof $(noinst_bindir)/src/programs/metaphor_profile ps.gmon.out > ps.profile
	
