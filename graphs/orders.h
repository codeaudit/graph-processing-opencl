/*
 *  orders.h
 *  graphs
 *
 *  Created by Alexandros Halatsis on 29/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#ifndef DEFINED_ORDERS
#define DEFINED_ORDERS

#define DEBUG_OUTPUT					/* print debugging data as-we-go to stdout */
#define DEBUG_OUTPUT_2					/* even more debug data */
#define PRINT_ERRORS					/* print errors to stderr */

#define PADDING_SIZE 4					/* for nice printouts */

#define GETMEMLIMITS_FAILURE 1			/* error getting limit values */
#define GETMEMLIMITS_SUCCESS 0

#define CHECKMEMLIMITS_FAILURE 1		/* problem getting memory limit */
#define CHECKMEMLIMITS_MEM_NOT_OK 1		/* memory usage estimate above limit */
#define CHECKMEMLIMITS_MEM_OK 0			/* memory usage estimate below limit */

#define READSOURCE_OPEN 1				/* error opening file */
#define READSOURCE_READ 1				/* error reading file */
#define READSOURCE_OK 0			

#define AVAILCLDEVICES_MEM 1			/* not enough memory */
#define AVAILCLDEVICES_LIST 1			/* could not get list of devices */
#define AVAILCLDEVICES_INFO 1			/* could not get info about devices */
#define AVAILCLDEVICES_OK 0

#define GETDEVICEINFO_INFO 1			/* error getting extended information abou device */
#define GETDEVICEINFO_OK 0

#define PARSENGCEOUTPUT_OPEN 1			/* error opening file */
#define PARSENGCEOUTPUT_MEM 1			/* not enough memory */
#define PARSENGCEOUTPUT_OK 0

#define BETWEENESS_FIFO_APPEND_MEM 1		/* not enough memory */
#define BETWEENESS_FIFO_APPEND_OK 0

#endif