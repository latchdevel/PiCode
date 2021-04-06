/*
 	PiCode Library https://github.com/latchdevel/PiCode

    Copyright (c) 2021 Jorge Rivera. All right reserved.
    License GNU Lesser General Public License v3.0.

  	Feb 2021: - Add int isNumeric(char *str);
*/

#include "common.h"

#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* strtod */

#include "log.h"

int isNumeric(char *s){
	logprintf(LOG_STACK, "%s(...)", __FUNCTION__);

	if(s == NULL|| *s == '\0' || *s == ' ')
		return -1;

	char *p = NULL;
	strtod(s, &p);
	return (*p == '\0') ? 0 : -1;
}