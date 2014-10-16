AC_INIT([package], [version])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])     # not even really needed
AC_GNU_SOURCE
AC_PROG_CC                        # or AC_PROG_CXX
AC_PROG_LIBTOOL
AC_CHECK_PROG(PS_EXISTS,"ps",1)
AC_PATH_PROG(PS_PATH,"ps")
AC_CHECK_PROG(CAT_EXISTS,"cat",1)
AC_PATH_PROG(CAT_PATH,"cat")
AC_CHECK_PROG(NE_EXISTS,"non_existant",1,0)
AC_SUBST(LIBTOOL_DEPS)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile metaphor.pc src/Makefile src/library/Makefile src/platforms/Makefile src/programs/Makefile tests/Makefile profile/Makefile])
LT_INIT
AC_OUTPUT
