#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	uint32_t magic;
	uint32_t count;
} dataHeader;

typedef struct {
	uint32_t magic1;
	uint16_t magic2;
	uint16_t useless;
	uint64_t count;
} compHeader;

typedef struct {
	uint16_t type;
	uint16_t rsrvd1;
	uint16_t rsrvd2;
	uint16_t rsrvd3;
	uint32_t offset1;
	uint32_t offset2;
} compElement;

const uint32_t dataMagic = 0x21796F4A;
const uint32_t compMagic1 = 0xAFBC7A37;
const uint16_t compMagic2 = 0x1C27;

int main(int argc, char** argv) {
	
	if (argc != 3) {
		errx(1, "Wrong usage");
	}

	int data = open(argv[1], O_RDWR);
	int comp = open(argv[2], O_RDONLY);

	//fd checks

	dataHeader dataHeaderRead;
	compHeader compHeaderRead;

	if (read(data, &dataHeaderRead, sizeof(dataHeader)) != (ssize_t)sizeof(dataHeader)) {
		err(2, "Error reading");
	}

	if (dataHeaderRead.magic != dataMagic) {
		errx(3, "Data header magic value does not match");
	}

	if (read(comp, &compHeaderRead, sizeof(compHeader)) != (ssize_t)sizeof(compHeader)) {
		err(2, "Error reading");
	}

	if (compHeaderRead.magic1 != compMagic1 || compHeaderRead.magic2 != compMagic2) {
		errx(4, "Comp header magic value does not match");
	}

	if (compHeaderRead.count == 0) {
		errx(5, "Nothing to compare");
	}

	uint8_t toSwitch;
	ssize_t readSize;
	uint32_t dataElCount = dataHeaderRead.count;
	uint64_t data1, data2;
	compElement compElRead;

	while ((readSize = read(comp, &compElRead, sizeof(compElement)) == (ssize_t)sizeof(compElement))) {
		
		toSwitch = 0;

		if (compElRead.rsrvd1 != 0 || compElRead.rsrvd2 != 0 || compElRead.rsrvd3 != 0) {
			errx(6, "Data element in comp match the specs");
		}

		if (compElRead.offset1 >= dataElCount || compElRead.offset2 >= dataElCount) {
			errx(7, "Comp file has offset too big for data file");
		}

		if (compElRead.offset1 == compElRead.offset2) {
			continue;	
		}

		if (lseek(data, 8 + compElRead.offset1 * sizeof(uint64_t), SEEK_SET) == -1) {
			err(8, "Error seeking");
		}

		if (read(data, &data1, sizeof(uint64_t)) != (ssize_t)sizeof(uint64_t)) {
			err(2, "Error reading");
		}
		
		if (lseek(data, 8 + compElRead.offset2 * sizeof(uint64_t), SEEK_SET) == -1) {
			err(8, "Error seeking");
		}

		if (read(data, &data2, sizeof(uint64_t)) != (ssize_t)sizeof(uint64_t)) {
			err(2, "Error reading");
		}

		if (compElRead.type == 0 && data1 > data2) {
			toSwitch = 1;
		} else if (compElRead.type == 1 && data1 < data2) {
			toSwitch = 1;
		} else if (compElRead.type != 0 && compElRead.type != 1) {
			errx(9, "Comp file has incorrect type value");	
		}

		if (toSwitch == 1) {
			
			if (write(data, &data1, sizeof(uint64_t) != (ssize_t)sizeof(uint64_t))) {
				err(9, "Error writing");
			}

			if (lseek(data, 8 + compElRead.offset1 * sizeof(uint64_t), SEEK_SET) == -1) {
				err(8, "Error seeking");
			}

			if (write(data, &data2, sizeof(uint64_t) != (ssize_t)sizeof(uint64_t))) {
				err(9, "Error writing");
			}
		}
	}

	exit(0);
}

