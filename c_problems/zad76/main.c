#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const uint32_t MOD = 4;
const uint32_t maxElements = 524288;
const char* newLine = "\n";
const char* endLine  = ";";
const char* tab = "\t";
const char* coma = ",";
const char* whitespace = " ";
const char* firstLine = "#include<stdint.h>\n\n";
const char* secondLine = "const uint32_t arrN = ";
const char* thirdLine = "const uint16_t arr[] = {";
const char* fourthLine = "};\n";

void writeSafe(const int fd, const char* string);
void writeNumSafe(const int fd, const uint16_t num);

void writeSafe(const int fd, const char* string) {
		if (write(fd, string, strlen(string)) != (ssize_t)strlen(string)) {
			err(2, "Error writing");
		}
}

void writeNumSafe(const int fd, const uint16_t num) {
	
	uint8_t size = 0;
	uint8_t counter = 0;
	uint16_t temp = num;

	while (temp > 0) {
		temp /= 10;
		size++;
	}

	char* toPrint = malloc(size + 1);

	temp = num;
	counter = size - 1;

	while (temp > 0) {
		toPrint[counter--] = temp % 10 + '0';
		temp /= 10;
	}

	toPrint[size] = '\0';

	writeSafe(fd, toPrint);
}

int main(int argc, char* argv[]) {

	if (argc != 3) {

		errx(1, "Wrong usage mf");
	}

	int input = open(argv[1], O_RDONLY);
	int output = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);

	if (input == -1) {
		err(1, "Could not open %s.", argv[1]);
	}

	if (output == -1) {
		err(1, "Could not open %s.", argv[2]);
	}

	off_t inputSize = lseek(input, 0, SEEK_END);

	if (inputSize == -1) {
		err(3, "Error seeking");
	}

	if (lseek(input, 0, SEEK_SET) == -1) {
		err(3, "Error seeking");
	}

	uint32_t numCount = inputSize / sizeof(uint16_t);

	//file integrity check with %

	if (numCount > maxElements) {
		errx(2, "Input file has too many entries");
	}

	writeSafe(output, firstLine);
	writeSafe(output, secondLine);
	writeNumSafe(output, numCount);
	writeSafe(output, endLine);
	writeSafe(output, newLine);
	writeSafe(output, thirdLine);

	uint16_t toRead;
	uint32_t counter = 0;

	while(read(input, &toRead, sizeof(uint16_t)) == sizeof(uint16_t)) {
		
		if (counter % MOD == 0) {
			writeSafe(output, newLine);
			writeSafe(output, tab);
		}

		writeNumSafe(output, toRead);

		if (counter + 1 < numCount) {

			writeSafe(output, coma);
			writeSafe(output, whitespace);
		}

		counter++;
	}

	writeSafe(output, fourthLine);

	close(input);
	close(output);
	
	exit(0);
}
