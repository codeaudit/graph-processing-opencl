/*
 *  sourceKernel.c
 *  graphs
 *
 *  Created by Alexandros Halatsis on 29/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "orders.h"
#include "sourceKernel.h"

/**
 * readKernelSource read a .c file into a char *
 *
 * opencl required the kernel function to be stored in a const char *
 * coding directly in such a format is a pain. this function reads a .c file
 * (where you can have proper syntax-highlighting etc) into a const char *
 *
 * @param err store error codes
 * @return the char array containing the source code
 */
char * readKernelSource(const char * filename, int *err)
{
	long int size;
	FILE *fin = fopen(filename, "r");
	if (fin == NULL)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Error (%d) opening kernel source file : %s\n", __FILE__, __LINE__, errno, strerror(errno), filename);
		#endif
		*err = READSOURCE_OPEN;
		return NULL;
	}
	fseek(fin, 0, SEEK_END);
	size = ftell(fin); /* file size, so we allocate (n + 1)-buffer byte and fill it */
	fseek(fin, 0, SEEK_SET);
	char *source = malloc((sizeof(char) * size) + 1);
	if (size != fread(source, sizeof(char), size, fin)) 
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Error reading kernel source file (%s)\n", __FILE__, __LINE__, filename);
		#endif
		free(source);
		*err = READSOURCE_READ;
		return NULL;
	} 
	fclose(fin);
	
	*err = READSOURCE_OK;	
	return source;
}