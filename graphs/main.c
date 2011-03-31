/*
 *  main.c
 *  graphs
 *
 *  Created by Alexandros Halatsis on 28/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <OpenCL/OpenCL.h>

#include "node.h"
#include "loadNGCE.h"
#include "limits.h"
#include "orders.h"
#include "clBasic.h"
#include "sourceKernel.h"
#include "betweeness.h"
#include "timing.h"

#define GRAPH_PATH "/Users/alex/ngcev2.0/GraphTopology500.txt"
#define KERNEL_FILE "/Users/alex/Documents/code/graphs/kernel.cl"

#define COMPUTE_OPENCL

int main()
{
	unsigned long int i, j; /* jokers */
	unsigned long long int k; /* another joker */
	int err;
	
	extern unsigned long int totalNodes; /* declared in loadNGCE.c */
	timexStart();
	node *graph = parseNGCEoutput(GRAPH_PATH, &err); /* create graph */
	if  (graph == NULL)
		return EXIT_FAILURE;
	fprintf(stdout, "Took %4.2f seconds to load graph from file\n", timexGet());

#ifdef COMPUTE_C
	/* computing betweness centrality on cpu */
	fprintf(stdout, "Computing shortest paths and betweeness centrality on cpu, coded in c\n");
	timexStart();
	signed long long int **computedSP = malloc(totalNodes * sizeof(signed long long int *));
	signed long long int pathLength;
	for (i = 0; i < totalNodes; i++)
	{
		computedSP[i] = malloc(totalNodes * sizeof(signed long long int)); /* initialization */
		for (j = 0; j < totalNodes; j++)
		{
			if (i == j)
				computedSP[i][j] = 0; /* 0 distance to self */
			else
				computedSP[i][j] = -1; /* unexplored path */
		}
	}
	
	for (i = 0; i < totalNodes; i++)
		for (j = 0; j < totalNodes; j++)
			if (computedSP[i][j] == -1) /* not known */
			{			
				if ((graph[i].peerCount == 0) || (graph[j].peerCount ==0))
				{} /* oops. no connection */
				else
					pathLength = shortestPath(&graph[i], &graph[j], graph); /* calculate */
				
				computedSP[i][j] = pathLength; /* set this */
				computedSP[j][i] = pathLength; /* and reverse to result */
			}
	fprintf(stdout, "Took %2f seconds to compute shortest paths and betweeness centrality\n", timexGet());
	
	#ifdef DEBUG_OUTPUT
		for (i = 0; i < totalNodes; i++)
			fprintf(stdout, "Node %4lu has a betweeness centrality of %4lu\n", graph[i].name, graph[i].spCount);
	#endif
	
#endif
	
#ifdef COMPUTE_OPENCL
	/* creating all shortest paths-pairs */
	k = 0;
	pair *all = malloc(totalNodes * 3 * sizeof(pair));
	for (i = 0; i < totalNodes; i++)
		for (j = 0; j < totalNodes; j++, k++)
		{
			all[k].a = i;
			all[k].b = j;
		}	
	
	/* choosing compute device */
	cl_device_id *devID = malloc(sizeof(cl_device_id));
	devID = availClDevices(&err);
	
	#ifdef DEBUG_OUTPUT
		clDeviceSpecs info;
		err = getDeviceInfo(&info, devID);
		fprintf(stdout, "%s:%d: Working on: %s %s. Memory: %lluM. Work items: %zu. Compute units: %d. Local memory (cache): %lluM. Maximum malloc: %lluM\n",
			__FILE__, __LINE__, info.vendor, info.name, info.memory/1024/1024, info.workGroupSize, info.compUnits, info.localMemory/1024/1024, info.maxMalloc/1024/1024);
	#endif	
	
	/* demo loading kernel source from text file */
	char *fnSource = readKernelSource(KERNEL_FILE, &err);
	char **kernelSource = malloc(sizeof(char *)); /* opencl bs */
	kernelSource[0] = fnSource;
	if (err != READSOURCE_OK)
	{
		free(graph);
		return EXIT_FAILURE;
	}		
	#ifdef DEBUG_OUTPUT
		fprintf(stdout, "%s:%d: Kernel source code following:\n%s\n", __FILE__, __LINE__, fnSource);
	#endif
	
	/* the context */
	cl_context context;
	context = clCreateContext(0, 1, devID, NULL, NULL, &err);
	if (err != CL_SUCCESS)
	{
		fprintf(stderr, "%s:%d: Error creating context: %s\n", __FILE__, __LINE__, strerrorCL(&err));
		return EXIT_FAILURE;
	}

	/* the command queue */
	cl_command_queue commands;
	commands = clCreateCommandQueue(context, *devID, 0, &err);
    if (!commands)
    {
        fprintf(stderr, "%s:%d: Error creating command queue: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }

	/* the actual program, checking */
	cl_program program;
	program = clCreateProgramWithSource(context, 1, (const char **) kernelSource, NULL, &err);
    if (!program)
    {
        fprintf(stderr, "%s:%d: Error creating program: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }
	
	/* the actual program, building */
	err = clBuildProgram(program, 1, devID, NULL, NULL, NULL); /* clBuildProgam(program, 0, NULL, NULL, NULL, NULL); */
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
		
        fprintf(stderr, "%s:%d: Error building program executable: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        clGetProgramBuildInfo(program, *devID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        fprintf(stderr, "%s:%d: Build log following: %s\n", __FILE__, __LINE__, buffer);
        return EXIT_FAILURE;
    }
	
	/* the kernel */
	cl_kernel kernel;
	kernel = clCreateKernel(program, "betweeness", &err);
	/* cl_kernel clCreateKernel(cl_program program, const char *kernel_name, cl_int *errcode_ret) */	 
    if (!kernel || err != CL_SUCCESS)
    {
        fprintf(stderr, "%s:%d: Error creating compute kernel: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }

	cl_mem inputGraph;
	cl_mem inputPairs;
    cl_mem outputLengths;
	size_t inputGraphSize = sizeof(node) * totalNodes;
	size_t inputPairsSize = sizeof(pair) * totalNodes * 3;
	size_t outputLengthsSize = sizeof(unsigned long int) * totalNodes;
	inputGraph = clCreateBuffer(context,  CL_MEM_READ_ONLY,  inputGraphSize, NULL, NULL);
	inputPairs = clCreateBuffer(context, CL_MEM_READ_ONLY, inputPairsSize, NULL, NULL);
    outputLengths = clCreateBuffer(context, CL_MEM_WRITE_ONLY, outputLengthsSize, NULL, NULL);
    if (!inputGraph || !inputPairs || !outputLengths)
    {
        fprintf(stderr, "%s:%d: Error allocating device memory: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }    
    
    err = clEnqueueWriteBuffer(commands, inputGraph, CL_TRUE, 0, inputGraphSize, graph, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "%s:%d: Error writing to source array: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }
	
	err = clEnqueueWriteBuffer(commands, inputPairs, CL_TRUE, 0, inputPairsSize, all, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "%s:%d: Error writing to source array: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }
	
    err = 0;
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputGraph);
	err  = clSetKernelArg(kernel, 1, sizeof(cl_mem), &inputPairs);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &outputLengths);
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned long int), &totalNodes);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "%s:%d: Error setting kernela arguments: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }
	
	size_t local;
    err = clGetKernelWorkGroupInfo(kernel, *devID, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "%s:%d: Error retrieving kernel work group info: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }
	
    unsigned long long int global = totalNodes * 3;
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
    if (err)
    {
		fprintf(stderr, "%s:%d: Error executing kernel: %s\n", __FILE__, __LINE__, strerrorCL(&err));
        return EXIT_FAILURE;
    }
	
    clFinish(commands);
	
	unsigned long int *betw = malloc(totalNodes * sizeof(unsigned long int));
    err = clEnqueueReadBuffer( commands, outputLengths, CL_TRUE, 0, sizeof(unsigned long int) * totalNodes, betw, 0, NULL, NULL );  
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array! %d\n", err);
        exit(1);
    }
	
	
	for (i = 0; i < totalNodes; i++)
		printf("computed a betweeness centrality of: %lu\n", betw[i]);
		
	free(graph);
	free(devID);
	free(kernelSource);
//	free(computedSP);
#endif
	
	
	return EXIT_SUCCESS;
}
