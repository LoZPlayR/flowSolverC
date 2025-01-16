.PHONY = all clean

CC := gcc
CC := ${CC}


SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

all: 
	@echo "Making everything..."
	${CC} -o flowsolver ${SRCS} -O3

debug:
	@echo "Making debug"
	${CC} -o flowsolver ${SRCS} -fsanitize=address -g

# %: %.o
# 	@echo "Checking.."
# 	${CC} -lm $< -o $@ -O3


# %.o: %.c
# 	@echo "Creating object.."
# 	${CC} -c $< -O3

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}

