#include <stdlib.h>

int __volatile sink;

int
main(int argc, char *argv[])
{
	int i;

	for (i = 0; i < 10000000; i++)
		sink = atoi("foo");

	return (1);
}
