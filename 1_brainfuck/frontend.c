#include "bf.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


void program_parse(struct program *p, const char *text) {
	size_t i = 0;
	uint32_t count = 0;
	char c;

	size_t bracket_stack[BF_MAX_BRACKET];
	size_t bracket_stack_len = 0;

	char prev_char = '\0';
	while (true) {
		c = text[i++];

		if (prev_char != c) {
			switch (prev_char) {
			case '+':
				program_push_op(p,
					(struct op) { .kind = OP_INC,
						      .data.count = count });
				break;

			case '-':
				program_push_op(p,
					(struct op) { .kind = OP_DEC,
						      .data.count = count });
				break;

			case ',':
				while (count--)
					program_push_op(p,
						(struct op) { .kind = OP_INPUT });
				break;

			case '.':
				while (count--)
					program_push_op(p,
						(struct op) { .kind = OP_OUTPUT });
				break;

			case '<':
				program_push_op(p,
					(struct op) { .kind = OP_MOVE_LEFT,
						      .data.count = count });
				break;

			case '>':
				program_push_op(p,
					(struct op) { .kind = OP_MOVE_RIGHT,
						      .data.count = count });
				break;

			case '[':
				bracket_stack[bracket_stack_len++] = p->len;

				program_push_op(p,
					(struct op) { .kind = OP_SZ });

				break;

			case ']': {
				size_t pair = bracket_stack[--bracket_stack_len];

				p->program[pair].data.bracket_pair = p->len;

				program_push_op(p,
					(struct op) { .kind = OP_SNZ,
						      .data.bracket_pair = pair });

				break;

			} default:
				break;
			}

			count = 0;
		}

		prev_char = c;
		count++;

		if (c == '\0')
			break;
	}
}

void program_init(struct program *p)
{
	assert(p->program = malloc(sizeof(struct op) * INITIAL_CAP));
	p->len = 0;
	p->cap = INITIAL_CAP;
}


void program_push_op(struct program *p, struct op op)
{
	if (p->len == p->cap) {
		p->cap *= 2;
		assert(p->program = realloc(p->program, sizeof(struct op) * p->cap));
	}

	p->program[p->len++] = op;
}
