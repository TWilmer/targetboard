/*
 * memset.c
 *
 *  Created on: 25-Sep-2015
 *      Author: thorsten
 */

#include <string.h>

void *memset(void *s, int c, size_t n)
{
	size_t i;
	unsigned char *dest=s;
	for(i=0;i<n;i++)
	{
		dest[i]=c;
	}
	return s;
}
