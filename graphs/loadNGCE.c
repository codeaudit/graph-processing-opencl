/*
 *  loadNGCE.c
 *  graphs
 *
 *  Created by Alexandros Halatsis on 28/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "loadNGCE.h"
#include "node.h"
#include "orders.h"
#include "limits.h"

unsigned long int totalNodes;

/**
 * parseFile will create nodes from parsing the file generated by ngce
 *
 * @param file The text file where ngce outputs its data
 * @param err storing error codes
 * @return An array of pointers to node objects
 */
node * parseNGCEoutput(const char *file, int *err)
{
	unsigned long int i; /* jokers */
	
	FILE *fin = fopen(file, "r");
	if (fin == NULL)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Error (%d) opening graph file: %s\n", __FILE__, __LINE__, errno, strerror(errno));
		#endif
		*err = PARSENGCEOUTPUT_OPEN;
		return NULL;
	}
	char buffer[MAX_NGCE_LINE];
	char *start, *end;
	
	/* reading first line, containing node count
	 * first line format: #Nodes:= NUMBER
	 */
	fgets(buffer, sizeof(buffer), fin);
	start = &buffer[8];
	totalNodes = strtol(start, &end, 10);
	#ifdef DEBUG_OUTPUT
		fprintf(stdout, "%s:%d: Going to load %lu nodes\n", __FILE__, __LINE__, totalNodes);
	#endif
	
	/* checking limits */
	if (checkMemLimits(&totalNodes) != CHECKMEMLIMITS_MEM_OK)
		fprintf(stderr, "%s:%d: Estimating that memory will not suffice. Proceeding nevertheless\n", __FILE__, __LINE__);
	
	/* reading another 4 lines of nonsense data */
	fgets(buffer, sizeof(buffer), fin);
	fgets(buffer, sizeof(buffer), fin);
	fgets(buffer, sizeof(buffer), fin);
	fgets(buffer, sizeof(buffer), fin);

	/* saving current position for later use */
	unsigned long int dataStartPos = ftell(fin);
	
	/* creating the graph, setting up basic node info */
	node *graph = malloc(sizeof(node) * totalNodes);
	if (graph == NULL)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Out of available memory: %s (error code=%d)\n", __FILE__, __LINE__, strerror(errno), errno);
		#endif
		*err = PARSENGCEOUTPUT_MEM;
		return NULL;
	}
	for (i = 0; i < totalNodes; i++) /* node initialization */
	{
		graph[i].name = i;
		graph[i].peerCount = 0;
		graph[i].spCount = 0;
	}
	
	/* first pass, counting how many peers each node has */
	int lhs, rhs;
	while (fgets(buffer, sizeof(buffer), fin))
	{
		start = buffer;
		lhs = strtol(start, &end, 10); /* from node */
		rhs = strtol(end, NULL, 10); /* to node */
		graph[lhs].peerCount++;
	}
	
	/* second pass, pointing to the peers */
	int nextP = 0, curNode = 0;
	fseek(fin, dataStartPos, SEEK_SET);
	while (fgets(buffer, sizeof(buffer), fin))
	{
		start = buffer;
		lhs = strtol(start, &end, 10);
		rhs = strtol(end, NULL, 10);
		
		if (graph[lhs].peers == NULL)
		{
			graph[lhs].peers = malloc(sizeof(struct node *) * (graph[lhs].peerCount));
			if (graph[lhs].peers == NULL)
			{
				#ifdef PRINT_ERRORS
					fprintf(stderr, "%s:%d: Out of available memory: %s (error code=%d)\n", __FILE__, __LINE__, strerror(errno), errno);
				#endif
				*err = PARSENGCEOUTPUT_MEM;
				return NULL;
			}
		}
		
		if (curNode != lhs)
		{
			curNode = lhs;
			nextP = 0;
		}
		graph[curNode].peers[nextP] = &graph[rhs];
		nextP++;
	}
	#ifdef DEBUG_OUTPUT
		unsigned long int j;
		for (i = 0; i < totalNodes; i++)
		{
			fprintf(stdout, "\nNode %4lu has %4lu connections: ", graph[i].name, graph[i].peerCount);
				for (j = 0; j < graph[i].peerCount; j++)
					fprintf(stdout, "%lu ", (*graph[i].peers[j]).name);
		}
		fprintf(stdout, "\n\n");
	#endif
	
	fclose(fin);
	
	*err = PARSENGCEOUTPUT_OK;
	return graph;
}