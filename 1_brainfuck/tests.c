#include "bf.h"

#include <stdlib.h>


int main(void)
{
	struct program p;

	program_init(&p);

	struct op op;

	op.data.count = 1; op.kind = OP_INC;
	program_push_op(&p, op);

	op.data.count = 2; op.kind = OP_DEC;
	program_push_op(&p, op);

	op.data.count = 1; op.kind = OP_INPUT;
	program_push_op(&p, op);

	program_parse(&p, "+++>.,<-");

	return EXIT_SUCCESS;
}
