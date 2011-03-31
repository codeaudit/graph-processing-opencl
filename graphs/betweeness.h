/*
 *  betweeness.h
 *  graphs
 *
 *  Created by Alexandros Halatsis on 29/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#ifndef DEFINED_CENTRALITY
#define DEFINED_CENTRALITY

#include "node.h"

/* fifo queue, add to the back, get and remove the first one */
typedef struct _fifoNode fifoNode;

struct _fifoNode
{
	fifoNode *next;
	const node *actualNode;
};

typedef struct _fifo fifo;

struct _fifo
{
	fifoNode *head;
	fifoNode *tail;
};


/* fifo operation functions */
node * getNextFromFIFO(fifo *queue);
int appendToFIFO(fifo *queue, const node *nextActual);


/* compute shortest path */
signed long long int shortestPath(const node *peer1, const node *peer2, node *graph);


/* store path */
typedef struct _path path;

struct _path
{
	unsigned long int *b;
};


#endif
