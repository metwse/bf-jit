#include "bf.h"

#include <assert.h>
#include <stddef.h>

#include <string.h>
#include <sys/mman.h>


#define p(x) program[i++] = x;


#define pm(str) \
	for (size_t __j = 0; str[__j]; __j++) \
		p(str[__j]);

#define pn(str, n) \
	for (size_t __j = 0; __j < n; __j++) \
		p(str[__j]);

#define p_u32(u32) \
	p((u32 >> 0) & 255); \
	p((u32 >> 8) & 255); \
	p((u32 >> 16) & 255); \
	p((u32 >> 24) & 255);

#define p_u64(u64) \
	p_u32(u64); \
	p((u64 >> 32) & 255); \
	p((u64 >> 40) & 255); \
	p((u64 >> 48) & 255); \
	p((u64 >> 56) & 255);


void (*program_compile(struct program *p))(void *mem)
{
	char *program = mmap(NULL, BF_PAGE_SIZE,
			     PROT_READ | PROT_WRITE | PROT_EXEC,
			     MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
			     -1, 0);

	size_t i = 0;

	size_t bracket_stack[BF_MAX_BRACKET];
	size_t bracket_stack_len = 0;

	pm("\x49\x89\xfa");  /* mov %rdi,%r10 */

	for (size_t op_i = 0; op_i < p->len; op_i++) {
		struct op op = p->program[op_i];

		switch (op.kind) {
		case OP_INC:
			pm("\x49\x81\x02");  /* add ...,(%r10)*/
			p_u32(op.data.count);

			break;

		case OP_DEC:
			pm("\x49\x81\x2a");  /* sub ...,(%r10)*/
			p_u32(op.data.count);

			break;

		case OP_MOVE_LEFT:
			pm("\x49\x81\xea");  /* sub ...,%r10*/
			p_u32(op.data.count);

			break;

		case OP_MOVE_RIGHT:
			pm("\x49\x81\xc2");  /* add ...,%r10*/
			p_u32(op.data.count);

			break;

		case OP_INPUT:
			/* 48 c7 c0 00 00 00 00    mov    $0x0,%rax
			 * 48 c7 c7 00 00 00 00    mov    $0x0,%rdi
			 * 4c 89 d6                mov    %r10,%rsi
			 * 48 c7 c2 01 00 00 00    mov    $0x1,%rdx
			 * 0f 05                   syscall */

			pm("\x48\xc7\xc0");
			p_u32(0);

			pm("\x48\xc7\xc7");
			p_u32(0);

			pm("\x4c\x89\xd6");

			pm("\x48\xc7\xc2");
			p_u32(1);

			pm("\x0f\x05");

			break;

		case OP_OUTPUT:
			/* 48 c7 c0 01 00 00 00    mov    $0x1,%rax
			 * 48 c7 c7 01 00 00 00    mov    $0x1,%rdi
			 * 4c 89 d6                mov    %r10,%rsi
			 * 48 c7 c2 01 00 00 00    mov    $0x1,%rdx
			 * 0f 05                   syscall */

			pm("\x48\xc7\xc0");
			p_u32(1);

			pm("\x48\xc7\xc7");
			p_u32(1);

			pm("\x4c\x89\xd6");

			pm("\x48\xc7\xc2");
			p_u32(1);

			pm("\x0f\x05");

			break;

		case OP_SZ:
			/*
			 * 48 b8 xx xx xx xx xx    movabs ...,%rax
			 * xx xx xx
			 * 49 83 3a 00             cmpq   $0x0,(%r10)
			 * 75 02                   jne    12 <skip>
			 * ff e0                   jmp    *%rax
			 */

			bracket_stack[bracket_stack_len++] = i;

			pm("\x48\xb8");
			p_u64(0LL);

			pn("\x49\x83\x3a\x00", 4);

			pm("\x75\x02");

			pm("\xff\xe0");

			break;

		case OP_SNZ: {
			/*
			 * 48 b8 xx xx xx xx xx    movabs ...,%rax
			 * xx xx xx
			 * 49 83 3a 00             cmpq   $0x0,(%r10)
			 * 74 02                   je     12 <skip>
			 * ff e0                   jmp    *%rax
			 */

			size_t pair = bracket_stack[--bracket_stack_len];
			char *pair_addr = &program[pair];
			char *this_addr = &program[i];

			pm("\x48\xb8");
			p_u64(((size_t) pair_addr));

			pn("\x49\x83\x3a\x00", 4);

			pm("\x74\x02");

			pm("\xff\xe0");

			size_t hold_i = i;

			i = pair + 2;
			p_u64(((size_t) this_addr));
			i = hold_i;

			break;
		}}
	}

	p('\xc3');  /* ret */

	void (*program_)(void *);

	memcpy(&program_, &program, sizeof(void *));

	return program_;
}
