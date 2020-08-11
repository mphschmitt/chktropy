COMPILER := gcc
MATH_FLAGS := -lm
ERROR_FLAGS := -Wall -Werror -Wextra
OPTIMIZATION_FLAGS := -O3

PROG_NAME := chktropy

SRC := main.c

chkentropy: ${SRC}
	gcc main.c ${MATH_FLAGS} ${ERROR_FLAGS} ${OPTIMIZATION_FLAGS} -o ${PROG_NAME}

# install:

# clean:
