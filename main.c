#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <float.h>
#include <getopt.h>

#define BUF_SIZE 128
#define ASCII_SIZE 128

enum arguments {
	ARGS_A = 0X01, /* Display all informations */
	ARGS_I = 0x02, /* Do not print input */
	ARGS_E = 0x04, /* Only print the entropy */
	ARGS_R = 0x08  /* Round entropy */
};

static void usage(void)
{
	printf(
		"Usage: chktropy [OPTIONS]\n"
		"Calculate the entropy of the ASCII password given on the standard input\n"
		"  -h  --help       display this help message\n"
		"  -i  --stdin      do not print stdin while reading it\n"
		"  -a  --all        display number of chars, unique chars,\n"
		"                     and the number possible passwords\n"
		"  -e  --entropy    display the entropy value only\n"
		"  -r  --round      round the entropy to the closest integer\n");
}

static double calculate_entropy(
	unsigned long int nb_unique_chars,
	unsigned long int nb_chars,
	double *nb_passwords)
{
	double entropy = 0.0;

	/* 0^0 = 1, so nb_passwords will never be 0 */
	*nb_passwords = pow((double)nb_unique_chars, (double)nb_chars);

	if (isinf(*nb_passwords))
		*nb_passwords = DBL_MAX;

	entropy = log(*nb_passwords);

	*nb_passwords = round(*nb_passwords);

	return entropy;
}

int main(int argc, char *argv[])
{
	char ascii[ASCII_SIZE] = {0};
	char buffer[BUF_SIZE] = {0};
	char args = 0;
	int opt;
	unsigned long int nb_unique_chars = 0;
	unsigned long int nb_chars = 0;
	double nb_passwords = 0.0;
	double entropy = 0.0;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"all", no_argument, 0, 'a'},
		{"round", no_argument, 0, 'r'},
		{"input", no_argument, 0, 'i'},
		{"entropy", no_argument, 0, 'e'},
		{0, 0, 0, 0}
	};

	// Support arbitrary precision numbers
	// Add support for long options
	while ((opt = getopt_long(argc, argv, "aeihr", long_options, NULL)) != -1) {
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
		case 'r':
			args |= ARGS_R;
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

	if (optind < argc) {
		printf("Invalid argument: %s\n", argv[1]);
		usage();
		exit(EXIT_FAILURE);
	}

	if (!(args & ARGS_I))
		printf("Input:              ");
	while (1) {
		ssize_t ret = 0;

		ret = read(STDIN_FILENO, buffer, sizeof(buffer));
		if (ret == 0) {
			break;
		} else if (ret < 0) {
			printf("%s\n", strerror(errno));
			usage();
			goto end;
		}

		nb_chars += ret;

		for (int i = 0; i < ret; ++i) {
			int c = (int)buffer[i];

			if (c < 0 || c > 127) {
				printf("Chktropy only supports 7 bits ASCII. This password cannot be handled\n");
				exit(EXIT_FAILURE);
			}

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

	entropy = calculate_entropy(nb_unique_chars, nb_chars, &nb_passwords);
	if (args & ARGS_R)
		entropy = round(entropy);

	// Definir des options d'output:
	// 		- Nombre de chiffres après la virgule pour l'entreopy

	if (args & ARGS_A) {
		printf(
			"Characters:         %lu\n"
			"Unique characters:  %lu\n"
			"Possible passwords: %.0f\n",
			nb_chars, nb_unique_chars, nb_passwords);
	}
	if (!(args & ARGS_E))
		printf("Entropy:            ");
	if (args & ARGS_R)
		printf("%.0f\n", entropy);
	else
		printf("%f\n", entropy);

end:
	exit(EXIT_SUCCESS);
}
