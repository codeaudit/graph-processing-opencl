/*
 *  node.h
 *  graphs
 *
 *  Created by Alexandros Halatsis on 28/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#ifndef NODE_DEFINED
#define NODE_DEFINED

typedef struct _node node;

struct _node
{
	unsigned long int name;			/* node id */
	unsigned long int peerCount;	/* how many connections the node has */
	node **peers;					/* one pointer for each connection the node has */
	float degree;					/* degree centrality */
	
	/* sp */
	node *parent;					/* pointing to the parent so we can trace the path backwards */
	unsigned int visited :1;		/* set if we visited, won't work in opencl, http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/restrictions.html */
	unsigned long int spCount;		/* how many shortest paths go through this vertex */
};

typedef struct _pair pair;

struct _pair
{
	unsigned long int a;
	unsigned long int b;
};

#endif