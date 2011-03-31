/*
 *  timing.c
 *  graphs
 *
 *  Created by Alexandros Halatsis on 07/12/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#include <stdio.h>
#include <time.h>

#include "timing.h"

clock_t start, finish;

/**
 * start timer
 */
void timexStart()
{
	start = clock();
}

/**
 * stop timer and return time passed since timexStart was executed 
 *
 * @return duration 
 */
double timexGet()
{
 	double duration;
 	finish = clock();
 	duration = (double)(finish - start)/CLOCKS_PER_SEC;
	return duration;
} 