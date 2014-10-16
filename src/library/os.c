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

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ftw.h>
#include <time.h>
#include <pthread.h>

#include "metaphor/third-party/uthash.h"
#include "metaphor/utils/stringutils.h"
#include "metaphor/os/processes.h"
#include "metaphor/os/filesystem.h"
#include "metaphor/os/os.h"

int initialized_metaphor = 0;
pthread_mutex_t resources_initialized_mutex;
__thread int initializing_resources = 0;

int
set_initializing_resources(int flag)
{
	initializing_resources = flag;
	return initializing_resources;
}

int
get_initializing_resources()
{
	return initializing_resources;
}

int
remap_resources(void)
{

	pthread_mutex_lock(&resources_initialized_mutex);
	if (initialized_metaphor == 0) {

		set_initializing_resources(1);
		provided_remap_resources_function();
		set_initializing_resources(0);
	}
	initialized_metaphor = 1;
	pthread_mutex_unlock(&resources_initialized_mutex);

	return 0;
}

void
set_remap_resources_function(remap_resources_function function)
{

	provided_remap_resources_function = function;
}
