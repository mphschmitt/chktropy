#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define BUF_SIZE 1024
#define ASCII_SIZE 128

static void usage(void)
{
	printf("Usage: \n");
	printf("    chkentropy <options>\n");
}

static double calculate_entropy(
	unsigned long int nb_unique_chars,
	unsigned long int nb_chars)
{
	double nb_possible_passwd;
	double entropy;

	nb_possible_passwd = 0.0;

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
	ssize_t total_bytes_read;
	unsigned long int nb_unique_chars;
	unsigned long int nb_chars;
	double entropy;

	nb_unique_chars = 0;
	nb_chars = 0;

	while (1) {
		ssize_t ret;
		ret = 0;

		ret = read(STDIN_FILENO, buffer, sizeof(buffer));
		if (ret == 0)
			break;
		else if (ret < 0) {
			printf("Error while reading from stdin... Is there any input?\n");
			usage();
			goto end;
		}

		nb_chars += ret;
		for (int i = 0; i < ret; ++i) {
			int c;

			c = (int)buffer[i];

			if (buffer[i] == '\n') {
				buffer[i] = '\0';
				nb_chars -= 1;
				goto entropy;
			}

			if (!ascii[c]) {
				ascii[c] = 1;
				nb_unique_chars += 1;
			}
		}

		memset(buffer, 0, sizeof(buffer));
		printf("read: %s\n", buffer);
	}

entropy:

	entropy = calculate_entropy(nb_unique_chars, nb_chars);

	// Definir des options d'output:
	// 		- tout
	// 		- Juste l'entropie
	// 		- Nombre de chiffres après la virgule pour l'entreopy
	// 		- Entreopy nombre entier
	// 		- Display input or not
	printf("input: %s\n", buffer);
	printf("nbr of chars: %u\n", nb_chars);
	printf("nbr of unqiue chars: %u\n", nb_unique_chars);
	printf("Entropy: %f\n", entropy);

end:
	return 0;
}
