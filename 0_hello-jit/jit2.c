#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>


#define JIT_MEM_SIZE 16 * 1024 * 1024


__asm__(
	".section jit_write,\"ax\",@progbits\n"
	".globl jit_write_\n"
	".type jit_write_, @function\n"
"jit_write_:\n"
	"mov %rdi,%rsi\n"
	"mov $1,%rax\n"
	"mov $1,%rdi\n"
	"mov $13,%rdx\n"
	"syscall\n"
	".text\n"
);

extern void __start_jit_write(void);
extern void __stop_jit_write(void);

__asm__(
	".section jit_ret,\"ax\",@progbits\n"
	".globl jit_ret_\n"
	".type jit_ret_, @function\n"
"jit_ret_:\n"
	"ret\n"
	".text\n"
);

extern void __start_jit_ret(void);
extern void __stop_jit_ret(void);


char *jit_c;
size_t jit_size = 0;

void jit_push(const char *c, size_t n)
{
	for (size_t i = 0; i < n; i++)
		jit_c[jit_size++] = c[i];
}

int main()
{
	void (*jit)(void *mem) = mmap(NULL, JIT_MEM_SIZE,
				      PROT_READ | PROT_WRITE,
				      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(jit != MAP_FAILED);

	void *mem = malloc(16 * 1024);
	assert(mem && "malloc fail");

	jit_c = (char *) jit;

	memcpy(mem, "hello, world\n", 13);

	jit_push((char *) &__start_jit_write, __stop_jit_write - __start_jit_write);
	jit_push((char *) &__start_jit_ret, __stop_jit_ret - __start_jit_ret);

	mprotect(jit, JIT_MEM_SIZE, PROT_EXEC);

	jit(mem);

	free(mem);
	munmap(jit, JIT_MEM_SIZE);
}
