/*
 *  limits.h
 *  graphs
 *
 *  Created by Alexandros Halatsis on 29/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#ifndef DEFINED_LIMITS
#define DEFINED_LIMITS

/* how much memory to add to the estimate, in bytes */
#define MEM_REST 1024

int getMemLimits();
int checkMemLimits(const unsigned long int *nodeCount);

#endif