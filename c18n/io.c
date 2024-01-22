#include <stdio.h>

#include "pw.h"

static char tocheck[12];

/* Read a password from stdin, badly. */
char *
readpassword(void)
{

	fscanf(stdin, "%s", tocheck);
	return (tocheck);
}
