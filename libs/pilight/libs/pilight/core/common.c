/*
  Modified by Jorge Rivera Feb 2021:
  - Add int isNumeric(char *str);
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