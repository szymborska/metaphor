# QUALITY

Please run `make quality` before commiting. It will run CMocka unit and
integration tests, valgrind for memory leaks, cppcheck for static analysis,
and gprof to profile the code. 
 
## TESTING REQUIREMENTS

Unit tests and integration tests in metaphor is tested using CMocka. Please 
provide unit tests for any submitted patches. 

Because metaphor intercepts system calls, helper programs must be used
in some cases to run the unit tests so that LD_PRELOAD can load a
metaphor library before running the unit tests. The program 
`run_metaphor_filesystem_tests` is a good example.

In addition to tests for correctness, there are also integration tests
that look for what common UNIX programs exist in the environment. Programs
like top and ps are run and checked for correctness.

All unit tests are run automatically when the code is built.

## MEMORY REQUIREMENTS

Memory leaks are checked using a combination of Valgrind and CMocka. The
program `metaphor_memory_tests` calls valgrind on the program 
`metaphor_memory_calls` and passes in an integer for the test function to
call. The test function then tests memory allocation in metaphor by 
allocating data structures ande calling functions. If -1 is passed to 
`metaphor_memory_calls` it returns the number of test functions. This
is used to loop through al of them in `metaphor_memory_tests`.

## SECURITY_REQUIREMENTS

** Minimal permissions **

Metaphor does not run with elevated permissions intentionally. 

** Safe string handling **

Only safe functions should be used throughout the codebase. Do *not* use:

(1) strcpy 
(2) strcat
(3) etc.

A set of string utilities in metaphor/stringutils.h has been provided. Please 
use these in place of writing your own string code.

** Authentication **

If authentication is required, please pass back the authentication to the 
user using the standard library functions if possible. We are looking into 
using a password manager for users who want a more seamless experience but 
this will have to be explicitly enabled. Longer term, holding a 
session - like MySQL - open and requiring reauthorization after a period 
of time, is also being investigated. 

## STATIC ANALYSIS

The static analysis tool `cppcheck` is automatically run when a build occurs and
exits if there is a failure. uthash, a third-party library used by metaphor 
currently has several warnings. Enough that analyzing includes automatically
has been disabled. The path forward is to fully abstract this library and
(probably) eliminate it as a dependency.

## PROFILING

Initial work on profiling has been started with gprof. More work needs to be done
here to make the profiling work across all integration commands and to exit on
performance regressions.

## THREAD SAFETY

For thread safety, metaphor uses a semaphore to lock the initialization of the
metaphor. 

## UNICODE SUPPORT

Metaphor supports unicode strings in its path operations. There are unit tests in the
test directory for these operations. Note this support will not translate to 
Windows (which has separate Unicode APIs).

## PLATFORMS

Metaphor currently only supports UNIX platforms. Support for other POSIX platforms
is planned.

