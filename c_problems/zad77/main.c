#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	uint16_t magic;
	uint16_t filetype;
	uint32_t count;
} Header;

typedef struct {
	uint16_t position;
	uint16_t value;
} listElement;

const uint8_t headerSize = 8;
const uint16_t filetype1 = 1;
const uint16_t filetype2 = 2;
const uint16_t filetype3 = 3;
const uint16_t MAGIC = 0x5A4D;

const size_t type1Size = sizeof(uint16_t);
const size_t type2Size = sizeof(uint32_t);
const size_t type3Size = sizeof(uint64_t);

int main(int argc, char** argv) {

	if (argc != 4) {
		errx(1, "Wrong usage");
	}

	int list = open(argv[1], O_RDONLY);
	int data = open(argv[2], O_RDONLY);
	int out = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);

	//fd checks

	Header toRead;

	if (read(list, &toRead, sizeof(Header)) != (ssize_t)sizeof(Header)) {
		err(2, "Could not read");
	}

	if (toRead.magic != MAGIC) {
		errx(3, "File is not part of Hoge");
	}

	if (toRead.filetype != 1) {
		errx(4, "List file filetype does not match");
	}

	if (toRead.count == 0) {
		errx(5, "List file has no data to read except the header");
	}

	if (read(data, &toRead, sizeof(Header)) != (ssize_t)sizeof(Header)) {
		err(2, "Could not read");
	}

	if (toRead.magic != MAGIC) {
		errx(3, "File is not part of Hoge");
	}

	if (toRead.filetype != 2) {
		errx(4, "List file filetype does not match");
	}

	ssize_t readSize;
	uint32_t maxValue = 0;
	listElement readEl;

	while ((readSize = read(list, &readEl, sizeof(listElement)) == (ssize_t)sizeof(listElement))) {
		
		uint32_t dataRead;
		
		if (readEl.position >= toRead.count) {
			errx(5, "List file contains position out of range for data file");
		}

		if (lseek(data, readEl.position * type2Size + headerSize, SEEK_SET) == -1) {
			err(6, "Error seeking");
		}

		if (read(data, &dataRead, type2Size) != (ssize_t)type2Size) {
			err(2, "Could not read");
		}

		uint64_t outWrite = (uint64_t)dataRead;

		maxValue = maxValue < readEl.value ? readEl.value : maxValue;

		if (lseek(out, readEl.value * type3Size + headerSize, SEEK_SET) == -1) {
			err(6, "Error seeking");
		}

		if (write(out, &outWrite, type3Size) != (ssize_t)type3Size) {
			err(7, "Could not write");
		}
	}

	if (readSize != 0) {
		err(2, "Could not read");
	}

	Header outHeader;

	outHeader.magic = MAGIC;
	outHeader.filetype = filetype3;
	outHeader.count = maxValue + 1;

	if (lseek(out, 0, SEEK_SET) == -1) {
		err(6, "Error seeking");
	}

	if (write(out, &outHeader, headerSize) != (ssize_t)headerSize) {
		err(7, "Could not write");
	}

	exit(0);
}
