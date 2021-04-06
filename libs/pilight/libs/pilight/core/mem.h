/*
	Copyright (C) 2013 - 2016 CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Copyright (c) 2021 Jorge Rivera. All right reserved. LGPL-3.0.
  April 2021: - Remove unused declarations.
*/

#ifndef _MEM_H_
#define _MEM_H_

#define OUT_OF_MEMORY fprintf(stderr, "out of memory in %s #%d\n", __FILE__, __LINE__),exit(EXIT_FAILURE);

#define MALLOC malloc
#define REALLOC realloc
#define CALLOC calloc
#define STRDUP strdup
#define FREE(a) free((void *)(a)),(a)=NULL

#endif
