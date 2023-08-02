#include <err.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	char c, lastChar;
	uint64_t counter = 0;

	if (argc != 3) {
		errx(1, "Wrong usage mf");
	}

	while(read(0, &c, sizeof(char)) == sizeof(char)) {
		if (strcmp(argv[1], "-d") == 0) {

			uint8_t hasChar = 0;

			for(char* curr = argv[2]; *curr != '\0'; curr++) {
				if (*curr == c) {
					hasChar = 1;

					break;
				}
			}

			if (hasChar == 0) {
				write(1, &c, 1);
			}
		} else if (strcmp(argv[1], "-s") == 0) {

			if (counter == 0) {
				write(1, &c, 1);
			} else {

				uint8_t hasChar = 0;

				for(char* curr = argv[2]; *curr != '\0'; curr++) {
					if (*curr == c) {
						hasChar = 1;

						break;
					}
				}

				if (hasChar == 0) {
					write(1, &c, 1);
				} else if (lastChar != c) {
					write(1, &c, 1);
				}
			}
		} else {
			//check for argv[1] == argv[2] sizes
			uint8_t hasChar = 0;

			for(char* curr = argv[1]; *curr != '\0'; curr++) {
				if (*curr == c) {
					write(1, argv[2] + (curr - argv[1]), 1);

					hasChar = 1;

					break;
				}
			}

			if (hasChar == 0) {
				write(1, &c, 1);
			}
		}

		counter++;
		lastChar = c;
	}

	exit(0);
}
