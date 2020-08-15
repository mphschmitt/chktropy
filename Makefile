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

OUTPUT_DIR := out
PROG_NAME := chktropy

SRC := main.c

chkentropy: ${SRC}
	@mkdir ${OUTPUT_DIR}
	@${COMPILER} main.c ${FLAGS} -o ${OUTPUT_DIR}/${PROG_NAME}

# install:

clean:
	@rm ${OUTPUT_DIR}/${PROG_NAME}
	@rmdir ${OUTPUT_DIR}
