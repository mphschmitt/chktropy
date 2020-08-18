/*
    chktropy checks the entropy of a password
    Copyright (C) 2020  Mathias Schmitt

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
	ARGS_R = 0x08, /* Round entropy */
	ARGS_S = 0x10  /* Argv input */
};

static void usage(void)
{
	printf(
		"Usage: chktropy [OPTIONS]\n"
		"Calculate the entropy of the ASCII password given on the standard input\n"
		"  -h  --help       display this help message and exit\n"
		"  -i  --stdin      do not print stdin while reading it\n"
		"  -a  --all        display number of chars, unique chars,\n"
		"                     and the number possible passwords\n"
		"  -e  --entropy    display the entropy value only\n"
		"  -r  --round      round the entropy to the closest integer\n"
		"  -s  --string     use the string passed as an argument instead of the stdin\n"
		"  -v  --version    output version information and exit\n");
}

static void version(void)
{
	printf(
	"cktropy 1.0.0\n"
	"\n"
	"Copyright (C) 2020 Mathias Schmitt\n"
	"License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
    "This is free software, and you are welcome to change and redistribute it\n"
    "This program comes with ABSOLUTELY NO WARRANTY.\n");
}

static long double calculate_entropy(
	unsigned long int nb_unique_chars,
	unsigned long int nb_chars,
	long double *nb_passwords)
{
	long double entropy = 0.0;

	/* 0^0 = 1, so nb_passwords will never be 0 */
	*nb_passwords = powl((double)nb_unique_chars, (double)nb_chars);

	if (isinf(*nb_passwords))
		*nb_passwords = LDBL_MAX;

	entropy = logl(*nb_passwords);

	*nb_passwords = roundl(*nb_passwords);

	return entropy;
}

static char check_arguments(int argc, char *argv[], char **input_str)
{
	char args = 0;
	int opt;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"all", no_argument, 0, 'a'},
		{"round", no_argument, 0, 'r'},
		{"input", no_argument, 0, 'i'},
		{"entropy", no_argument, 0, 'e'},
		{"string", required_argument, 0, 's'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "aeihrs:v", long_options, NULL)) != -1) {
		switch (opt) {
		case 'a':
			if (args & ARGS_E) {
				printf("Incompatible options: --all and --entropy\n");
				usage();
				return -EINVAL;
			}
			args |= ARGS_A;
			break;
		case 'i':
			args |= ARGS_I;
			break;
		case 'e':
			if (args & ARGS_A) {
				printf("Incompatible options: --all and --entropy\n");
				usage();
				return -EINVAL;
			}
			args |= ARGS_E;
			args |= ARGS_I;
			break;
		case 'r':
			args |= ARGS_R;
			break;
		case 's':
			args |= ARGS_S;
			*input_str = strndup(optarg, strlen(optarg));
			break;
		case 'v':
			version();
			return -EINVAL;
		case 'h':
		case '?':
			usage();
			return -EINVAL;
		default:
			break;
		}
	}

	if (optind < argc) {
		printf("Invalid argument: %s\n", argv[1]);
		usage();
		return -EINVAL;
	}

	return args;
}

static int buff_count(char *str, size_t length,
	unsigned long int *nb_unique_chars, unsigned long int *nb_chars, char args)
{
	char ascii[ASCII_SIZE] = {0};
	size_t i;

	for (i = 0; i < length; ++i) {
		int c = (int)str[i];

		if (c < 0 || c > 127) {
			printf("Chktropy only supports 7 bits ASCII. This password cannot be handled\n");
			return -EINVAL;
		}

		if (str[i] == '\n') {
			str[i] = '\0';

			*nb_chars -= 1;
			if (!(args & ARGS_I))
				printf("%s", str);
			break;
		}

		if (!ascii[c]) {
			ascii[c] = 1;
			*nb_unique_chars += 1;
		}
	}

	return 0;
}

static int handle_string_arg(char *str, unsigned long int *nb_unique_chars,
	unsigned long int *nb_chars, char args)
{
	size_t length;
	int res;

	length = strlen(str);
	res = buff_count(str, length, nb_unique_chars, nb_chars, args);
	if (res)
		return -EINVAL;

	if (!(args & ARGS_I))
		printf("%s", str);

	return 0;
}

int main(int argc, char *argv[])
{
	char buffer[BUF_SIZE] = {0};
	char args;
	char *input_str = NULL;
	unsigned long int nb_unique_chars = 0;
	unsigned long int nb_chars = 0;
	long double nb_passwords = 0.0;
	long double entropy = 0.0;
	int res;

	args = check_arguments(argc, argv, &input_str);
	if (args < 0)
		goto end;

	if (!(args & ARGS_I))
		printf("Input:              ");

	if (args & ARGS_S) {
		if (!input_str) {
			printf("-s option requires a input string\n");
			usage();
			goto end;
		}
		nb_chars = strlen(input_str);
		res = handle_string_arg(input_str, &nb_unique_chars, &nb_chars, args);
		if (res)
			goto end;
	} else {
		while (1) {
			ssize_t ret = 0;

			ret = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
			if (ret == 0) {
				break;
			} else if (ret < 0) {
				printf("%s\n", strerror(errno));
				usage();
				goto end;
			}

			nb_chars += (unsigned long int)ret;

			res = buff_count(buffer, (unsigned long int)ret, &nb_unique_chars,
				&nb_chars, args);
			if (res)
				goto end;
			if (buffer[ret - 1] == '\0' || buffer[ret - 1] == '\n')
				goto entropy;

			if (!(args & ARGS_I))
				printf("%s", buffer);

			memset(buffer, 0, sizeof(buffer));
		}
	}

entropy:

	if (!(args & ARGS_I))
		printf("\n");

	entropy = calculate_entropy(nb_unique_chars, nb_chars, &nb_passwords);
	if (args & ARGS_R)
		entropy = roundl(entropy);

	if (args & ARGS_A) {
		printf(
			"Characters:         %lu\n"
			"Unique characters:  %lu\n"
			"Possible passwords: %s%.0Lf\n",
			nb_chars, nb_unique_chars,
			nb_passwords == LDBL_MAX ? "more than " : "", nb_passwords);
	}
	if (!(args & ARGS_E))
		printf("Entropy:            ");
	if (args & ARGS_R)
		printf("%s%.0Lf\n", nb_passwords == LDBL_MAX ? "more than " : "", entropy);
	else
		printf("%s%Lf\n", nb_passwords == LDBL_MAX ? "more than " : "", entropy);

end:
	if (input_str) {
		free(input_str);
		input_str = NULL;
	}

	return 0;
}
