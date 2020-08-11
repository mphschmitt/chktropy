#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define BUF_SIZE 10
#define ASCII_SIZE 128

enum arguments {
	ARGS_A = 0X01,
	ARGS_I = 0x02,
	ARGS_E = 0x04
};

static void usage(void)
{
	printf("Usage: chktropy [OPTIONS]\n");
	printf("Calculate the entropy of the password given on the standard input\n");
	printf("  -i  --stdin      do not print stdin while reading it\n");
	printf("  -a  --all        display all informations\n");
	printf("  -e  --entropy    display the entropy only\n");
}

static double calculate_entropy(
	unsigned long int nb_unique_chars,
	unsigned long int nb_chars)
{
	double nb_possible_passwd = 0.0;
	double entropy = 0.0;

	// Gérer les cas de retours (overflow nombre, NaN)
	nb_possible_passwd = pow((double)nb_unique_chars, (double)nb_chars);
	// Gérer les cas de retours (overflow nombre, NaN)
	entropy = log(nb_possible_passwd);

	return entropy;
}

int main(int argc, char *argv[])
{
	char buffer[BUF_SIZE] = {0};
	char ascii[ASCII_SIZE] = {0};
	int opt;
	unsigned long int nb_unique_chars = 0;
	unsigned long int nb_chars = 0;
	double entropy = 0.0;
	char args = 0;

	// opterr = 0;

	while ((opt = getopt(argc, argv, "aeih")) != -1) {
		switch (opt) {
		case 'a':
			args |= ARGS_A;
			break;
		case 'i':
			args |= ARGS_I;
			break;
		case 'e':
			args |= ARGS_E;
			args |= ARGS_I;
			break;
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
		case '?':
			usage();
			exit(EXIT_FAILURE);
		default:
			break;
		}
	}

	if (!(args & ARGS_I))
		printf("input: ");
	while (1) {
		ssize_t ret = 0;

		ret = read(STDIN_FILENO, buffer, sizeof(buffer));
		if (ret == 0) {
			break;
		} else if (ret < 0) {
			// What could this error be? Write it in the message
			printf("Error while reading from stdin.\n");
			usage();
			goto end;
		}

		nb_chars += ret;

		for (int i = 0; i < ret; ++i) {
			int c = (int)buffer[i];

			if (buffer[i] == '\n') {
				buffer[i] = '\0';
				nb_chars -= 1;
				if (!(args & ARGS_I))
					printf("%s", buffer);
				goto entropy;
			}

			if (!ascii[c]) {
				ascii[c] = 1;
				nb_unique_chars += 1;
			}
		}

		if (!(args & ARGS_I))
			printf("%s", buffer);
		memset(buffer, 0, sizeof(buffer));
	}

entropy:

	if (!(args & ARGS_I))
		printf("\n");
	entropy = calculate_entropy(nb_unique_chars, nb_chars);

	// Definir des options d'output:
	// 		- Nombre de chiffres après la virgule pour l'entreopy
	// 		- Entreopy nombre entier

	if (args & ARGS_A) {
		printf("Characters: %lu\n", nb_chars);
		printf("Unique characters: %lu\n", nb_unique_chars);
	}
	if (!(args & ARGS_E))
		printf("Entropy: ");
	printf("%f\n", entropy);

end:
	return 0;
}
