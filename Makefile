.PHONY = all clean

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

all: 
	@echo "Making Flow Solver"
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c zobrist.c -O3

	@echo "Making Level Editor"
	gcc -o leveleditor leveleditor.c levelpack.c -O3 -lncurses

debug:
	@echo "Making debug flow solver"
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c zobrist.c -O0 -fsanitize=address -g

	@echo "Making debug level editor" 
	gcc -o leveleditor leveleditor.c levelpack.c -O0 -fsanitize=address -g -lncurses

valgrind:
	@echo "Making debug flow solver"
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c -O0 zobrist.c -g

	@echo "Making debug level editor" 
	gcc -o leveleditor leveleditor.c levelpack.c -O0 -g -lncurses


solver:
	@echo "Making Flow Solver"
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c zobrist.c -O3

editor:
	@echo "Making Level Editor"
	gcc -o leveleditor leveleditor.c levelpack.c -O3 -lncurses

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}

