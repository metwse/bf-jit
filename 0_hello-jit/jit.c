#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <sys/mman.h>


char *jit_c;
size_t jit_size = 0;

void pushc(char *c, size_t n)
{
	for (size_t i = 0; i < n; i++)
		jit_c[jit_size++] = c[i];
}

void push(char c)
{
	jit_c[jit_size++] = c;
}

int main()
{
	void (*jit)(void *mem) = mmap(NULL, 16 * 1024 * 1024,
				      PROT_READ | PROT_EXEC | PROT_WRITE,
				      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(jit != MAP_FAILED);

	void *mem = malloc(16 * 1024);
	assert(mem && "malloc fail");

	jit_c = (char *) jit;

	memcpy(mem, "hello, world", 13);

	pushc("\x48\x89\xfe", 3); // mov rsi,rdi
	pushc("\x48\xc7\xc0\x01\x00\x00\x00", 7);  // mov rax,1
	pushc("\x48\xc7\xc7\x00\x00\x00\x00", 7);  // mov rdi,0
	pushc("\x48\xc7\xc2\x0d\x00\x00\x00", 7); // mov rbx,1
	pushc("\x0f\x05", 2); // syscall
	push('\xc3'); // ret

	jit(mem);

	free(mem);
	munmap(jit, 16 * 1024 * 1024);
}
