#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
	uint32_t magic;
	uint8_t headerVersion;
	uint8_t dataVersion;
	uint16_t dataCount;
	uint32_t reserved1;
	uint32_t reserved2;
} header;

typedef struct {
	uint16_t offset;
	uint8_t origByte;
	uint8_t newByte;
} dataV0;

typedef struct {
	uint32_t offset;
	uint16_t origByte;
	uint16_t newByte;
} dataV1;

const uint32_t MAGIC_CONST = 0xDEADBEEF;

int main(int argc, char* argv[]) {

	if (argc != 4) {
		errx(1, "Wrong usage...");
	}

	int patch = open(argv[1], O_RDONLY);
	int fd1 = open(argv[2], O_RDONLY);
	int fd2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0666);

	//fd checks

	header patchHeader;

	if (read(patch, &patchHeader, sizeof(header)) != sizeof(header)) {
		err(2, "Error reading");
	}

	if (patchHeader.magic != MAGIC_CONST) {
		errx(3, "Patch file does not follow the file regulations");
	}

	//printf("HEADER: %ld|%d|%d|%d|%d|%d\n", patchHeader.magic, patchHeader.headerVersion, patchHeader.dataVersion, patchHeader.dataCount, patchHeader.reserved1, patchHeader.reserved2);

	if (patchHeader.dataVersion != 0 && patchHeader.dataVersion != 1) {
		errx(4, "data version var is not correct");
	}

	off_t fd1Size = lseek(fd1, 0, SEEK_END);

	//fd1Size check for -1

	if (lseek(fd1, 0, SEEK_SET) == -1) {
		err(3, "Error seeking");
	}

	uint8_t* fd1Data = malloc(fd1Size);

	if (!fd1Data) {
		err(4, "Error allocating mem");
	}

	if (read(fd1, fd1Data, fd1Size) != fd1Size) {
		err(2, "Error reading");
	}

	close(fd1);

	if (write(fd2, fd1Data, fd1Size) != fd1Size) {
		err(5, "Error writing");
	}
	 free(fd1Data);

	if (patchHeader.dataVersion == 0) {

		dataV0 toRead;
		uint8_t byteRead;
		off_t readSize, readSize2;

		while ((readSize = read(patch, &toRead, sizeof(dataV0))) != sizeof(dataV0)) {
			if (toRead.offset * sizeof(uint8_t) >= (uint64_t)fd1Size) {
				errx(5, "Patch file containts offset too big for file 1 && 2");	
			}

			if (lseek(fd2, toRead.offset * sizeof(uint8_t), SEEK_SET) == -1) {
				err(3, "Error seeking");
			}

			if ((readSize2 = read(fd2, &byteRead, sizeof(uint8_t))) != sizeof(uint8_t)) {
				err(2, "Error reading");
			}

			if (byteRead != toRead.origByte) {
				errx(5, "Patch file containts offset too big for file 1 && 2");	
			}

			if (lseek(fd2, toRead.offset * sizeof(uint8_t), SEEK_SET) == -1) {
				err(3, "Error seeking");
			}

			if (write(fd2, &toRead.newByte, sizeof(uint8_t)) != sizeof(uint8_t)) {
				err(5, "Error writing");
			}
		}

		if (readSize != 0) {
			err(2, "Error reading");
		}

	} else {

		dataV1 toRead;
		uint16_t byteRead;
		off_t readSize, readSize2;

		while ((readSize = read(patch, &toRead, sizeof(dataV1))) != sizeof(dataV1)) {
			if (toRead.offset * sizeof(uint16_t) >= (uint64_t)fd1Size) {
				errx(5, "Patch file containts offset too big for file 1 && 2");	
			}

			if (lseek(fd2, toRead.offset * sizeof(uint16_t), SEEK_SET) == -1) {
				err(3, "Error seeking");
			}

			if ((readSize2 = read(fd2, &byteRead, sizeof(uint16_t))) != sizeof(uint16_t)) {
				err(2, "Error reading");
			}

			if (byteRead != toRead.origByte) {
				errx(5, "Patch file containts offset too big for file 1 && 2");	
			}

			if (lseek(fd2, toRead.offset * sizeof(uint16_t), SEEK_SET) == -1) {
				err(3, "Error seeking");
			}

			if (write(fd2, &toRead.newByte, sizeof(uint16_t)) != sizeof(uint16_t)) {
				err(5, "Error writing");
			}
		}

		if (readSize != 0) {
			err(2, "Error reading");
		}
	}

	close(fd2);
	close(patch);

	exit(0);
}
