.PHONY = all clean

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

all: 
	@echo "Making Flow Solver"
	gcc -o flowsolver board.c move_gen.c levelpack.c solver.c bitmap.c hashtable.c logic.c eval.c -O3
# zobrist.c visited_set.c blackspace_analysis.c

	@echo "Making Level Editor"
	gcc -o leveleditor leveleditor.c levelpack.c -O3 -lncurses

debug:
	@echo "Making debug flow solver"
#	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c zobrist.c visited_set.c blackspace_analysis.c -O0 -fsanitize=address -g
	gcc -o flowsolver board.c move_gen.c levelpack.c solver.c bitmap.c hashtable.c logic.c eval.c -O0 -fsanitize=address -g
	@echo "Making debug level editor" 
	gcc -og leveleditor leveleditor.c levelpack.c -O0 -fsanitize=address -g -lncurses

valgrind:
	@echo "Making debug flow solver"
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c -O0 -g

	@echo "Making debug level editor" 
	gcc -o leveleditor leveleditor.c levelpack.c -O0 -g -lncurses


solver:
	@echo "Making Flow Solver"
	gcc -o flowsolver board.c eval.c gen_moves.c levelpack.c solver.c zobrist.c visited_set.c -O3

editor:
	@echo "Making Level Editor"
	gcc -o leveleditor leveleditor.c levelpack.c -O3 -lncurses

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}

