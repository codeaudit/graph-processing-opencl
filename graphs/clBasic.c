/*
 *  clBasic.c
 *  graphs
 *
 *  Created by Alexandros Halatsis on 29/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#include <OpenCL/OpenCL.h>
#include <stdio.h>
#include <stdlib.h>

#include "clBasic.h"
#include "orders.h"

/**
 * availClDevices prints out information about all available compute devices and lets the user choose
 *
 * Gets a list of all available opencl-enabled devices, and for each devices gets vendor and model name
 * so user can choose
 *
 * @param ret 4 possible value indicating execution status
 * @return cl_device_id object related to the compute device the user chose
 */
cl_device_id * availClDevices(int *err)
{
	int clErr, devID;
	cl_uint presentDevices;
	char buffer[CL_INFO_BUFFER];
	cl_device_id *chosen, *devices = malloc(sizeof(cl_device_id) * (CL_MAX_DEVICES));
	if (devices == NULL)
	{
		*err = AVAILCLDEVICES_MEM;
		return NULL;
	}
	
	clErr = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, CL_MAX_DEVICES, devices, &presentDevices); /* list of all devices, in unknown order (gpu or cpu first */
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get a list of available compute devices\n", __FILE__, __LINE__);
		#endif
		*err = AVAILCLDEVICES_LIST;
		return NULL;
	}
	fprintf(stdout, "Available compute devices(%d):\n", presentDevices);
	
	for (int i = 0; i < presentDevices; i++)
	{
		clErr = clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), &buffer, NULL); /* basic info about device, for identification. Eg: Intel */
		if (clErr != CL_SUCCESS)
		{
			#ifdef PRINT_ERRORS
				fprintf(stderr, "%s:%d: Failed to get DEVICE_VENDOR information about device with id %d\n", __FILE__, __LINE__, i);
			#endif
			*err = AVAILCLDEVICES_INFO;
			return NULL;
		}
		fprintf(stdout, "%d: %s", i, buffer);
		clErr = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), &buffer, NULL); /* basic info about device, for identification. Eg: Core2 Duo */
		if (clErr != CL_SUCCESS)
		{
			#ifdef PRINT_ERRORS
				fprintf(stderr, "%s:%d: Failed to get DEVICE_NAME information about device with id %d\n", __FILE__, __LINE__, i);
			#endif
			*err = AVAILCLDEVICES_INFO;
			return NULL;
		}
		fprintf(stdout, " %s\n", buffer);
	}
	
	fprintf(stdout, "Choose compute device: ");
	fscanf(stdin, "%d", &devID);
	fprintf(stdout, "\n");
	
	chosen = malloc(sizeof(cl_device_id));
	*chosen = devices[devID];
	free(devices);
	
	*err = AVAILCLDEVICES_OK;
	return chosen;
}

/**
 * getDeviceInfo returns a struct containing the available global memory, # of work items, compute units, local (cache) memory, maximum malloc size, name and vendor string
 *
 * @param specs will be assigned to a clDeviceSpecs struct containing the above
 * @param dev a cl_device_id describing the chosen device
 * @return 2 possible values indicating execution status
 */
int getDeviceInfo(clDeviceSpecs *specs, const cl_device_id *dev)
{
	int clErr, err;
	clDeviceSpecs info;
	
	/* CL_DEVICE_GLOBAL_MEM_SIZE: total memory available */
	clErr = clGetDeviceInfo(*dev, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(info.memory), &info.memory, NULL);
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get CL_DEVICE_GLOBAL_MEM_SIZE information about device\n", __FILE__, __LINE__);
		#endif
		specs = NULL;
		err = GETDEVICEINFO_INFO;
		return err;
	}
	
	/* CL_DEVICE_MAX_WORK_GROUP_SIZE: total work items available */
	clErr = clGetDeviceInfo(*dev, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(info.workGroupSize), &info.workGroupSize, NULL);
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get CL_DEVICE_MAX_WORK_GROUP_SIZE information about device\n", __FILE__, __LINE__);
		#endif
		specs = NULL;
		err = GETDEVICEINFO_INFO;
		return err;
	}
	
	/* CL_DEVICE_NAME: model */
	clErr = clGetDeviceInfo(*dev, CL_DEVICE_NAME, sizeof(info.name), &info.name, NULL);
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get CL_DEVICE_NAME information about device\n", __FILE__, __LINE__);
		#endif
		specs = NULL;
		err = GETDEVICEINFO_INFO;
		return err;
	}
	
	/* CL_DEVICE_VENDOR: manufacturer */
	clErr = clGetDeviceInfo(*dev, CL_DEVICE_VENDOR, sizeof(info.vendor), &info.vendor, NULL);
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get CL_DEVICE_VENDOR information about device\n", __FILE__, __LINE__);
		#endif
		specs = NULL;
		err = GETDEVICEINFO_INFO;
		return err;
	}
	
	/* CL_DEVICE_LOCAL_MEM_SIZE: local cache */
	clErr = clGetDeviceInfo(*dev, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(info.localMemory), &info.localMemory, NULL);
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get CL_DEVICE_LOCAL_MEM_SIZE information about device\n", __FILE__, __LINE__);
		#endif
		specs = NULL;
		err = GETDEVICEINFO_INFO;
		return err;
	}
	
	/* 	CL_DEVICE_MAX_COMPUTE_UNITS: compute units */
	clErr = clGetDeviceInfo(*dev, 	CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(info.compUnits), &info.compUnits, NULL);
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get CL_DEVICE_MAX_COMPUTE_UNITS information about device\n", __FILE__, __LINE__);
		#endif
		specs = NULL;
		err = GETDEVICEINFO_INFO;
		return err;
	}	
	
	/* 	CL_DEVICE_MAX_MEM_ALLOC_SIZE: maximum memory object allocation */
	clErr = clGetDeviceInfo(*dev, 	CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(info.maxMalloc), &info.maxMalloc, NULL);
	if (clErr != CL_SUCCESS)
	{
		#ifdef PRINT_ERRORS
			fprintf(stderr, "%s:%d: Failed to get CL_DEVICE_MAX_MEM_ALLOC_SIZE information about device\n", __FILE__, __LINE__);
		#endif
		specs = NULL;
		err = GETDEVICEINFO_INFO;
		return err;
	}	
	
	*specs = info;
	return err;
}

/**
 * Provide a human explanation to a opencl error code, according to
 * http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/errors.html
 *
 * @param err the error code
 */
char * strerrorCL(const cl_int *errnoCL)
{
	char *desc = malloc(sizeof(char) * CL_ERR_DESC); /* nice memory leak, I don't free this  anywhere */
	switch (*errnoCL)
	{
		case CL_SUCCESS: sprintf(desc, "Completed succesfully (CL_SUCCESS)");
			break;
		case CL_COMPILER_NOT_AVAILABLE: sprintf(desc, "No compiler available (CL_COMPILER_NOT_AVAILABLE)");
			break;
		case CL_DEVICE_NOT_AVAILABLE: sprintf(desc, "Device not available (CL_DEVICE_NOT_AVAILABLE)");
			break;
		case CL_INVALID_ARG_SIZE: sprintf(desc, "Argument size specified (arg_size) does not match the size of the data type for an argument (CL_INVALID_ARG_SIZE)");
			break;
		case CL_INVALID_ARG_VALUE: sprintf(desc, "Argument value specified is NULL for an argument that is not declared with the __local qualifier (CL_INVALID_ARG_VALUE)");
			break;
		case CL_INVALID_BUILD_OPTIONS: sprintf(desc, "Invalid build options (CL_INVALID_BUILD_OPTIONS)");
			break;
		case CL_INVALID_CONTEXT: sprintf(desc, "Invalid contect, refer to calling function for more information (CL_INVALID_CONTEXT)");
			break;
		case CL_INVALID_DEVICE: sprintf(desc, "Invalid device (CL_INVALID_DEVICE)");
			break;
		case CL_INVALID_KERNEL_NAME: sprintf(desc, "Specified kernel (function) name not found in cl_program (CL_INVALID_KERNEL_NAME)");
			break;
		case CL_INVALID_KERNEL: sprintf(desc, "Invalid kernel, refer to calling function for more information (CL_INVALID_KERNEL)");
			break;
		case CL_INVALID_KERNEL_ARGS: sprintf(desc, "Invalid kernel arguments (CL_INVALID_KERNEL_ARGS)");
			break;
		case CL_INVALID_PROGRAM: sprintf(desc, "Invalid program, refer to calling function for more information (CL_INVALID_PROGRAM)");
			break;
		case CL_INVALID_WORK_DIMENSION: sprintf(desc, "Invalid work dimensions, work_dim not set (CL_INVALID_WORK_DIMENSION)");
			break;
		case CL_INVALID_WORK_GROUP_SIZE: sprintf(desc, "Number of workitems specified by global_work_size is not evenly divisible by size of work-group given by local_work_size (CL_INVALID_WORK_GROUP_SIZE)");
			break;
		case CL_INVALID_WORK_ITEM_SIZE: sprintf(desc, "The number of work-items specified in any of local_work_size[] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[] (CL_INVALID_WORK_ITEM_SIZE)");
			break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE: sprintf(desc, "Failure to allocate memory for data store associated buffer objects specified as arguments to kernel (CL_MEM_OBJECT_ALLOCATION_FAILURE)");
			break;
		default: sprintf(desc, "An unknown-undocumented error occured");
	}
	return desc;
}