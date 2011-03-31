/*
 *  loadNGCE.h
 *  graphs
 *
 *  Created by Alexandros Halatsis on 28/11/2010.
 *  Copyright 2010 . All rights reserved.
 *
 */

#ifndef loadNGCE_DEFINED
#define loadNGCE_DEFINED

#define MAX_NGCE_LINE 128

#include "node.h"

node * parseNGCEoutput(const char *file, int *err);

#endif