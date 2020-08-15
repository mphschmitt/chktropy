COMPILER := gcc
MATH_FLAGS := -lm
ERROR_FLAGS := -Wall \
				-Werror \
				-Wextra \
				-pedantic \
				-Wshadow \
				-Wdouble-promotion \
				-fno-common \
				-Wconversion
FORMAT_FLAGS := -Wformat=2 -Wundef
OPTIMIZATION_FLAGS := -O3
FLAGS := ${MATH_FLAGS} ${ERROR_FLAGS} ${FORMAT_FLAGS} ${OPTIMIZATION_FLAGS}

PROG_NAME := chktropy

SRC := main.c

chkentropy: ${SRC}
	${COMPILER} main.c ${FLAGS} -o ${PROG_NAME}

# install:

# clean:
