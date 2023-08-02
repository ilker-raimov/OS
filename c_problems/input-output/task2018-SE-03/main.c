#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

const char* optionC = "-c";
const char* optionD = "-d";
const char* rangedArg = "-";

void writeSafe(char c);
void printChars(char* line, uint8_t startIndex, uint8_t endIndex);
void printCols(char* line, char* delimeter, uint8_t startIndex, uint8_t endIndex);

void writeSafe(char c) {
	if (write(1, &c, sizeof(c)) != sizeof(c)) {
		err(4, "Could not write to stdout.");
	}
}

void printChars(char* line, uint8_t startIndex, uint8_t endIndex) {
	
	for (uint16_t i = startIndex - 1; i + 1 <= endIndex; i++) {

		writeSafe(line[i]);
	}

	writeSafe('\n');
}

void printCols(char* line, char* delimeter, uint8_t startCol, uint8_t endCol) {
	uint16_t counter = 1, startIndex = 0, endIndex = 0;

	for(uint16_t i = 0; line[i] != '\0'; i++) {

		if (strcmp(&line[i], delimeter) == 0) {

			if (endCol == counter) {
				endIndex = i - 1;
			}

			counter++;

			if (startCol == counter) {
				startIndex = i + 1;
			}
		}
	}

	if (startCol <= counter && endCol > counter) {
		for(uint16_t i = startIndex; line[i] != '\0'; i++) {
			writeSafe(line[i]);
		}
	} else if (startCol <= counter && endCol <= counter) {
		for(uint16_t i = startIndex; i <= endIndex; i++) {
			writeSafe(line[i]);
		}
	}

	writeSafe('\n');
}

int main(int argc, char* argv[]) {
	
	if (argc < 3) {
		errx(1, "Wrong usage mf!");
	}

	ssize_t bytesRead;
	char buff[4096];

	uint8_t isRanged = strlen(argv[2]) == 1 ? 0 : 1;
	uint8_t startIndex, endIndex;

	if (strcmp(argv[1], optionC) == 0) {
	
		if (argc != 3) {
			errx(2, "Wrong usage - ./main -c x(-y) where x < y");
		}

		//validate sec arg
		isRanged = strlen(argv[2]) == 1 ? 0 : 1;

		if (isRanged == 0) {
			startIndex = (uint8_t)(argv[2][0] - '0');
			endIndex = startIndex;

		} else {
			startIndex = (uint8_t)(argv[2][0] - '0');
			endIndex = (uint8_t)(argv[2][2] - '0');

			if (endIndex <= startIndex) {
				errx(3, "First char num has to be less than the second char num");
			}
		}

		while((bytesRead = read(0, buff, sizeof(buff))) > 0) {
			buff[bytesRead] = '\0';

			printChars(buff, startIndex, endIndex);
		}
	} else if (strcmp(argv[1], optionD) == 0) {

		if (argc != 5) {
			errx(4, "Wrong usage again");
		}

		//check args -f exists && 2 is chars

		isRanged = strlen(argv[4]) == 1 ? 0 : 1;

		if (isRanged == 0) {

			startIndex = (uint8_t)(argv[4][0] - '0');
			endIndex = startIndex;
		} else {

			startIndex = (uint8_t)(argv[4][0] - '0');
			endIndex = (uint8_t)(argv[4][2] - '0');
		}

		while((bytesRead = read(0, buff, sizeof(buff))) > 0) {
			buff[bytesRead] = '\0';

			printCols(buff, &argv[2][0], startIndex, endIndex);
		}

	} else {
		errx(1, "Wrong usage mf!");
	}


	exit(0);	
}
