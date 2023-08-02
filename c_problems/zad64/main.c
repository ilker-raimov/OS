#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

typedef struct {
	uint16_t offset;
	uint8_t origByte;
	uint8_t newByte;
} elem;


int main(int argc, char* argv[]) {
	if (argc != 4) {
		errx(1, "Wrong usage!");
	}

	int patch = open(argv[1], O_RDONLY);
	int fd1 = open(argv[2], O_RDONLY);
	int fd2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0600);

	//fd checks

	uint8_t buff[256];
	ssize_t readSize;

	while ((readSize = read(fd1, &buff, sizeof(buff))) > 0) {
		//write checl

		write(fd2, &buff, readSize);
	}

	//id readSize !=0

	elem toRead;
	uint8_t byte;
	ssize_t fd2Size = lseek(fd2, 0, SEEK_CUR);

	while (read(patch, toRead, sizeof(elem)) == sizeof(elem)) {
		//check fd2size and offset
		lseek(fd2, toRead.offset, SEEK_SET);

		read(fd2, byte, sizeof(uint8_t));

		if (byte == toRead.origByte) {
			lseek(fd2, toRead.offset, SEEK_SET);

			write(fd2, toRead.newByte, sizeof(uint8_t));
		}
	}

	//read checks

	exit(0);
}
