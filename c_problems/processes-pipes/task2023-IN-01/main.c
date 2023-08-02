#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

const int mod = 3;
const int base = 10;
const char* L[] = {"tic ", "tac ", "toe\n"};

void writeSafe(const char* toWrite, int fd);

void writeSafe(const char* toWrite, int fd) {
	ssize_t size = strlen(toWrite);

	if (write(fd, toWrite, size) != size) {
		err(4, "Error writing");
	}
}

int main(int argc, char** argv) {

	if (argc != 3) {
		errx(1, "Wrong usage");
	}

	char* endptr1 = malloc(sizeof(char));
	char* endptr2 = malloc(sizeof(char));
	int pc = strtol(argv[1], &endptr1, base);
	int wc = strtol(argv[2], &endptr2, base);

	if (endptr1 == argv[1] || *endptr1 != '\0' || pc < 1 || pc > 7)  {
		errx(2, "Process count is an invalid number");
	} 

	if (endptr2 == argv[2] || *endptr2 != '\0' || wc < 1 || wc > 35)  {
		errx(3, "Word count is an invalid number");
	} 

	int counter = 0;

	while (counter < wc) {

		writeSafe(L[counter++ % mod], 1);

		if (counter == wc) {
			break;
		}

		for (int i = 0; i < pc; i++) {
			
			const pid_t child = fork();

			if (child == -1) {
				err(5, "Could not fork");
			}

			if (child == 0) {

				writeSafe(L[counter % mod], 1);

				exit(0);
			}

			counter++;

			int status;

			if (wait(&status) == -1) {
				err(6, "Could not wait for child to finish");
			}

			if (!WIFEXITED(status)) {
				err(7, "Child did not terminate properly");
			}

			if (WEXITSTATUS(status) != 0) {
				err(8, "Child did not exit with 0");
			}

			if (counter == wc) {
				break;
			}
		}

		if (counter == wc) {
			break;
		}
	}
	
	exit(0);
}
