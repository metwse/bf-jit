#ifndef BF_H
#define BF_H


#include <stddef.h>
#include <stdint.h>


#define BF_PAGE_SIZE 1024LL * 1024LL * 1024LL * 2LL


enum op_kind {
	OP_INC,
	OP_DEC,
	OP_MOVE_LEFT,
	OP_MOVE_RIGHT,
	OP_INPUT,
	OP_OUTPUT,
};

union op_data {
	uint32_t count;
};

struct op {
	enum op_kind kind;
	union op_data data;
};

struct program {
	struct op *program;
	size_t cap;
	size_t len;
};


void (*program_compile(struct program *))(void *mem);

void program_parse(struct program *, const char *);

void program_init(struct program *);

void program_push_op(struct program *, struct op);


#endif
