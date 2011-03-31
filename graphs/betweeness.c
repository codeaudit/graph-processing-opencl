/*
 *  betweeness.c
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

#include "betweeness.h"
#include "orders.h"
#include "node.h"

/**
 * gets head node from fifo and also removes it from the queue
 *
 * @param queue the queue on which to operate
 * @return the data node
 */
node * getNextFromFIFO(fifo *queue)
{
	if ((*queue).head == NULL)
		return NULL; /* empty fifo */
	
	node *tmp;
	fifoNode *n, *del;
	tmp = (*(*queue).head).actualNode;
	del = (*queue).head;
	n = (*(*queue).head).next;
	(*queue).head = n;
	free(del);
	return tmp;
}

/**
 * appends a node to the queue.
 *
 * @param queue the queue on which to operate
 * @param nextActual the node object to which the queue node points
 * @return int indicating status
 */
int appendToFIFO(fifo *queue, const node *nextActual)
{
	fifoNode *new = malloc(sizeof(fifoNode));
	if (new == NULL)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Out of available memory: %s (error code=%d)\n", __FILE__, __LINE__, strerror(errno), errno);
		#endif
		return BETWEENESS_FIFO_APPEND_MEM;
	}
	(*new).actualNode = nextActual;
	(*new).next = NULL;

	/* if empty queue */
	if ((*queue).head == NULL)
	{
		(*queue).head = new;
		(*queue).tail = new;
		return BETWEENESS_FIFO_APPEND_OK;
	}
	
	(*(*queue).tail).next = new;
	(*queue).tail = (*(*queue).tail).next;
	
	return BETWEENESS_FIFO_APPEND_OK;
}

/**
 * returning the shortest path distance between two nodes 
 *
 * @param peer1 the first node
 * @param peer2 the second node
 * @param graph the whole graph thing, used for adding a credit for each node use in a path
 * @return path distance
 */
signed long long int shortestPath(const node *peer1, const node *peer2, node *graph)
{
	#ifdef DEBUG_OUTPUT
		fprintf(stdout, "New shortestpath(), starting from %lu searching for %lu\n", (*peer1).name, (*peer2).name);
	#endif
	
	int err, found = 0;
	unsigned long int i; /* jokers */
	signed long long int distance = 0;
	fifo queue;
	queue.head = NULL;
	queue.tail = NULL;
	err = appendToFIFO(&queue, peer1); /* examining starts from peer1 */

	node *currentBase, *p;
	const node *tracer;
	currentBase = getNextFromFIFO(&queue); /* query the queue, and get peer1 back */

	while ((!found) && (currentBase != NULL)) /* haven't found and got more nodes to explore */
	{
		#ifdef DEBUG_OUTPUT_2
			fprintf(stdout, "\tExamining %lu\n", (*currentBase).name);
		#endif
		(*currentBase).visited = 1; /* so we don't queue it up again */
		for (i = 0; i < (*currentBase).peerCount; i++) 
		{
			p = ((*currentBase).peers[i]);
			if ((*p).name == (*peer2).name) /* found */
			{
				(*p).parent = currentBase; /* set parent so we can trace */
				found = 1;
				break;
			}
			else
			{
				if (((*p).peerCount > 0) && ((*p).visited == 0)) /* no need to add to queue if it doesn't have peers or is already visited */
				{
					#ifdef DEBUG_OUTPUT_2
						fprintf(stdout, "Adding %lu to queue\n", (*p).name);
					#endif
					(*p).visited == 1; /* so we don't queue it up again */
					(*p).parent = currentBase; /* trace */
					appendToFIFO(&queue, p);
				}
			}
		}
		currentBase = getNextFromFIFO(&queue); /* get next node for examination. returns NULL if nothing's left, while() catches it */
	}
	
	if (found)
	{
		/* walking the path, in reverse */
		tracer = peer2; /* start from peer2 */
		#ifdef DEBUG_OUTPUT
			fprintf(stdout, "\t");
		#endif
		while (tracer != peer1) /* while we haven't reached peer1 */
		{
			#ifdef DEBUG_OUTPUT
				fprintf(stdout, "%lu->", (*tracer).name);
			#endif			
			tracer = (*tracer).parent; /* next is our parent */
			distance++; /* and we walked another block */
			
			/* in betweeness centrality we are interested in how many shortest paths pass through a node.
			 * so for each node path passes through, we add a credit. that's why we need the graph object */
			if (tracer != peer2)
			{
				graph[(*tracer).name].spCount++;
			}			
		}
		#ifdef DEBUG_OUTPUT
			fprintf(stdout, "%lu\nComputed shortest path: %lli\n\n", (*peer1).name, distance);
		#endif
		return distance;
	}
	else
	{
		#ifdef DEBUG_OUTPUT
			fprintf(stdout, "No shortest path\n\n");
		#endif
		return -1; /* no possible path, so -1 (error) */
	}
}

