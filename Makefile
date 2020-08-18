# chktropy checks the entropy of a password
# Copyright (C) 2020  Mathias Schmitt
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

.PHONY: all
all: chktropy

.PHONY: chktropy
chktropy: ${SRC}
	@mkdir -p ${OUTPUT_DIR}
	@${COMPILER} main.c ${FLAGS} -o ${OUTPUT_DIR}/${PROG_NAME}

# .PHONY: install
# install:

.PHONY: clean
clean:
	@rm -rf ${OUTPUT_DIR}

.PHONY: help
help:
	@echo "Use one of the following targets:"
	@echo "  help     Print this help message"
	@echo "  all      Build chktropy"
	@echo "  clean    Clean output from previous build"
	@echo "  install  Install chktropy on your system"
