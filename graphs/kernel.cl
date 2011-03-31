typedef struct _node node;

struct _node
{
	unsigned long int name;			
	unsigned long int peerCount;	
	node **peers;				
	float degree;					
	
	/* sp */
	node *parent;					
	unsigned int visited :1;		
	unsigned long int spCount;		
}; 
 
typedef struct _pair pair;

struct _pair
{
	unsigned long int a;
	unsigned long int b;
}; 
 
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

int appendToFIFO(fifo *queue, const node *nextActual)
{
	fifoNode *new = malloc(sizeof(fifoNode));
	if (new == NULL)
	{
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
 
__kernel void betweeness(__global node *input, __global pair *p, __global unsigned long int *output, const unsigned long int totalNodes)
{
	int i = get_global_id(0);
	if (i < totalNodes * 3)
	{
		output[i] = (*p).b;
	}

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
		while (tracer != peer1) /* while we haven't reached peer1 */
		{		
			tracer = (*tracer).parent; /* next is our parent */
			distance++; /* and we walked another block */
			
			/* in betweeness centrality we are interested in how many shortest paths pass through a node.
			 * so for each node path passes through, we add a credit. that's why we need the graph object */
			if (tracer != peer2)
			{
				graph[(*tracer).name].spCount++;
			}			
		}
		return distance;
	}
	else
	{
		return -1; /* no possible path, so -1 (error) */
	}
}
