MODE ?= release

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

# All files except for the one containing executable main function.
NONMAIN_OBJS = $(filter-out main.o,$(OBJS))
# All files except for the one containing test executable main function.
NONTEST_OBJS = $(filter-out tests.o,$(OBJS))


EXTRAFLAGS_debug = -O0 -g3
EXTRAFLAGS_release = -O2 -Werror
EXTRAFLAGS_test = --coverage -O0 -g3

CFLAGS = -std=c11 -Wall -Wextra -pedantic -MMD -MP $(EXTRAFLAGS_$(MODE))


default: bf-jit


%.o: %.c
	$(CC) $(CFLAGS) -c $<

bf-jit: $(NONTEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

tests: $(NONMAIN_OBJS)
	$(CC) $(CFLAGS) $^ -o $@


clean:
	$(RM) *.o *.d *.gcno *.gcda bf-jit tests

.PHONY: clean default


-include $(OBJS:.o=.d)
