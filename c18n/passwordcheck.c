#include <string.h>

#include "pw.h"

static char the_password[] = "Password123";

/* Return 1 if the password is correct; otherwise, 0. */
int
passwordcheck(const char *tocheck)
{

	if (strncmp(tocheck, the_password, sizeof(the_password)))
		return (0);
	return (1);
}
