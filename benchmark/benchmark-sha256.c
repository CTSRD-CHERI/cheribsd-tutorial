#include <stdlib.h>
#include <sha256.h>

static const char buffer[1024];
int __volatile sink;

int
main(int argc, char *argv[])
{
	SHA256_CTX ctx;
	int i;

	SHA256_Init(&ctx);
	for (i = 0; i < 100000; i++)
		SHA256_Update(&ctx, buffer, sizeof(buffer));

	return (1);
}
