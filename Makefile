.PHONY = all clean

CC := gcc
CC := ${CC}


SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

all: 
	@echo "Making Flow Solver"
#	${CC} -o flowsolver ${SRCS} -O3
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c -O3

	@echo "Making Level Editor"
	gcc -o leveleditor leveleditor.c levelpack.c -O3 -lncurses

debug:
	@echo "Making debug flow solver"
#	${CC} -o flowsolver ${SRCS} -fsanitize=address -g
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c -O0 -fsanitize=address -g

	@echo "Making debug level editor"
	gcc -o leveleditor leveleditor.c levelpack.c -O0 -fsanitize=address -g -lncurses

solver:
	@echo "Making Flow Solver"
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c -O3

editor:
	@echo "Making Level Editor"
	gcc -o leveleditor leveleditor.c levelpack.c -O3 -lncurses

# %: %.o
# 	@echo "Checking.."
# 	${CC} -lm $< -o $@ -O3


# %.o: %.c
# 	@echo "Creating object.."
# 	${CC} -c $< -O3

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}

