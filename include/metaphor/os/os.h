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

#ifndef METAPHOR_OS_H
#define METAPHOR_OS_H

#include <metaphor/utils/hashutils.h>

typedef int (*remap_resources_function) (void);
remap_resources_function provided_remap_resources_function;

int set_initializing_resources(int flag);
int get_initializing_resources(void);
int remap_resources(void);
void set_remap_resources_function(remap_resources_function function);

#endif
