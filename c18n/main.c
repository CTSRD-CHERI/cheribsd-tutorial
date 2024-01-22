#include <stdio.h>
#include <unistd.h>

#include "pw.h"

static char the_secret[] = "VerySecretKey";

int
main(void)
{

	printf("PID: %d\n", getpid());
	printf("Password: ");
	fflush(stdout);
	if (passwordcheck(readpassword()))
		printf("The secret: %s\n", the_secret);
	else
		printf("Password incorrect\n");
	return (1);
}
