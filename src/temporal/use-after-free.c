#include <assert.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	char * __volatile cp;

	cp = malloc(sizeof(*cp));
	assert(cp != NULL);

	/* Use while allocated. */
	*cp = 'a';

	/* Use after free. */
	free(cp);
	*cp = 'b';

	/* Use after synchronous revocation. */
	/* malloc_revoke(); */
	*cp = 'c';

	exit(1);
}
