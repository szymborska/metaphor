ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tests

#####################################################################
#
# Disabled (external hash library has too many errors): cppcheck --enable=all --suppress=unusedFunction --suppress=missingIncludeSystem --error-exitcode=1 -I include src
# Perhaps, it should be replaced.
#
#####################################################################

generate_docs:
	cd docs; make man info 

install_docs:
	cd docs/build/texinfo; gzip metaphor.info; cp -f metaphor.info.gz $(datarootdir)/info
	cd docs/build/man; cp -f metaphor.1 $(datarootdir)/man/man1

install_metaphor:
	mkdir -p $(datarootdir)/metaphor
	cp configuration/locations.json $(datarootdir)/metaphor
	mv $(bindir)/metaphor_install $(bindir)/metaphor
	ln -s $(bindir)/metaphor $(bindir)/@ 
	
static_analysis:
	cppcheck --enable=all --suppress=unusedFunction --suppress=missingIncludeSystem --suppress=missingInclude --error-exitcode=1 -i src/library/m_syscall.c src

profile_code:
	cd profile; make all

run_tests:
	cd tests; make run_tests 

quality:
	make static_analysis 
	make run_tests
	make profile_code

install_link:
	ln -s $(bindir)/metaphor $(bindir)/@ 
	
install-exec-hook:
	make install_docs
	make install_metaphor

