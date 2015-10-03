/*
 * memcpy.c
 *
 *  Created on: 25-Sep-2015
 *      Author: thorsten
 */



#include <string.h>

void *memcpy(void *dest, const void *src, size_t len)
{
	char *d = (char *) dest;
	char *s = (char *) src;
	while (len--)
	{
		*d++ = *s++;
	}

	return dest;
}
