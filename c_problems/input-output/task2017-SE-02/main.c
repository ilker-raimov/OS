#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

const char* STDIN = "-";
const char* numberingOption = "-n";

void printFile(const char* filename, int32_t* lineCounter, bool doNumbering);

void printFile(const char* filename, int32_t* lineCounter, bool doNumbering) {
	char c;

	//if file size > 0

	if (doNumbering == true) {
		printf("%d ", *lineCounter);

		(*lineCounter)++;
	}

	if (*filename == *STDIN) {
		while(read(0, &c, sizeof(char)) == sizeof(char)) {
			printf(&c);

			if (c == '\n' && doNumbering == true) {
				printf("%d ", *lineCounter);

				(*lineCounter)++;
			}
		}
	} else {
		int fd = open(filename, O_RDONLY);
		//check fd
		
		while(read(fd, &c, sizeof(char)) == sizeof(char)) {
			printf(&c);

			if (c == '\n' && doNumbering == true) {
				printf("%d ", *lineCounter);

				(*lineCounter)++;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	bool doNumbering = false;
	int32_t lineCounter = 1;

	if (argc > 1) {
		doNumbering = *argv[1] == *numberingOption ? true : false;
	}

	if (argc <= 1 + doNumbering) {
		printFile(STDIN, &lineCounter, doNumbering);
	} else {
		for (int32_t i = 1 + doNumbering; i < argc; i++) {
			printFile(argv[i], &lineCounter, doNumbering);
		}
	}

	exit(0);
}

