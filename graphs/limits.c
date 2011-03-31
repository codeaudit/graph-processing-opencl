/*
 *  limits.c
 *  graphs
 *
 *  Created by Alexandros Halatsis on 29/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "limits.h"
#include "node.h"
#include "orders.h"


/**
 * getMemLimits gets the current memory limits
 *
 * @param limit limit struct to populate
 * @return 2 possible return values indicating successful or unsuccessful execution
 */
int getMemLimits(struct rlimit *limit)
{
	/* maximum virtual memory */
	if (getrlimit(RLIMIT_DATA, limit) != 0)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: getrlimit() failed with: %s", __FILE__, __LINE__, strerror(errno));
		#endif
		return GETMEMLIMITS_FAILURE;
	}
	#ifdef DEBUG_OUTPUT
		fprintf(stdout, "%s:%d: Current RLIMIT_DATA: %llu\t Maximum RLIMIT_DATA:%llu\n", __FILE__, __LINE__, (*limit).rlim_cur, (*limit).rlim_max);
	#endif
	return GETMEMLIMITS_SUCCESS;
}

/**
 * checkMemLimits will parses the struct rlimit from getMemLimits, estimates memory usage and returns if it suffices
 *
 * @param nodeCount number of total nodes to be loaded
 * @return 3 possbile return values indicating a problem, sufficient memory, insufficient memory
 */
int checkMemLimits(const unsigned long int *nodeCount)
{
	struct rlimit limit;
	int err = getMemLimits(&limit);
	
	if (err == GETMEMLIMITS_FAILURE)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, ":%s:%d: getMemLimits returned GETMEMLIMITS_FAILURE. Returning CHECKMEMLIMITS_FAILURE\n", __FILE__, __LINE__);
		#endif
		return CHECKMEMLIMITS_FAILURE;
	}
	
	/* the actual nodes and the arrays holding pointers to those nodes */
	unsigned long long int estimate = MEM_REST + ((*nodeCount) * sizeof(node)) + ((*nodeCount) * sizeof(node *));
	#ifdef DEBUG_OUTPUT
		fprintf(stdout, "%s:%d: Estimated mem usage: %llu\n", __FILE__, __LINE__, estimate);
	#endif
	return (limit.rlim_cur > estimate ? CHECKMEMLIMITS_MEM_OK : CHECKMEMLIMITS_MEM_NOT_OK);
}
	
