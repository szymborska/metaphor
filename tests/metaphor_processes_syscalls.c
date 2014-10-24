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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <metaphor/intercept/intercept_calls.h>
#include "metaphor_processes_tests.h"

SYSCALL_DEFINITIONS(initialize_metaphors)
