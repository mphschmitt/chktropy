SRC := main.c

chkentropy: ${SRC}
	gcc main.c -lm -o chkentropy

# Rajouter installation
install:
