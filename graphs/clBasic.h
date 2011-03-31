/*
 *  clBasic.h
 *  graphs
 *
 *  Created by Alexandros Halatsis on 29/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#ifndef DEFINED_CLBASIC
#define DEFINED_CLBASIC

/* maximum number of devices that we are going to store info for */
#define CL_MAX_DEVICES 5

/* maximum chars for vendor & name information */
#define CL_INFO_BUFFER 64

/* opencl errors in a human language. max description in characters */
#define CL_ERR_DESC 256

cl_device_id * availClDevices(int *err);

typedef struct _clDeviceSpecs clDeviceSpecs;

struct _clDeviceSpecs
{
	cl_ulong memory;
	size_t workGroupSize;
	char name[CL_INFO_BUFFER];
	char vendor[CL_INFO_BUFFER];
	cl_ulong localMemory;
	cl_uint compUnits;
	cl_ulong maxMalloc;
};

int getDeviceInfo(clDeviceSpecs *specs, const cl_device_id *dev);
char * strerrorCL(const cl_int *errnoCL);

#endif