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

#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>

#include "metaphor/intercept/intercept_calls.h"

int initialize_metaphors(void);
int
initialize_metaphors(void)
{
	return 0;
}

SYSCALL_DEFINITIONS(initialize_metaphors)
