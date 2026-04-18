#include "bf.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>


int main(void)
{
	struct program p;

	program_init(&p);

	char buf[4096];
	buf[0] = '\0';

	while (fgets(buf, 4096, stdin)) {
		program_parse(&p, buf);
	}

	void *mem = calloc(1, 1024 * 1024 * 4);

	void (*jit)(void *mem) = program_compile(&p);

	jit(mem);

	free(mem);
	munmap(jit, BF_PAGE_SIZE);

	return EXIT_SUCCESS;
}
