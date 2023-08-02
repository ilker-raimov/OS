#include <sys/types.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

typedef struct {
	uint16_t offset;
	uint8_t length;
	uint8_t ffs;
} elem;

const uint8_t upperStart = 0x41;
const uint8_t upperEnd = 0x5A;

int main(int argc, char* argv[]) {

	if (argc != 5) {
		errx(1, "WRONGH USAGE AMINA KOYUM");
	}

	int fdat1 = open(argv[1], O_RDONLY);
	int fidx1 = open(argv[2], O_RDONLY);
	int fdat2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0600);
	int fidx2 = open(argv[4], O_CREAT | O_TRUNC | O_RDWR, 0600);

	//checks for fds

	elem toRead;
	uint8_t emptyByte = 0x00;
	ssize_t read_size;
	uint16_t lastStart = 0, lastSize = 0;

	while((read_size = read(fidx1, &toRead, sizeof(toRead))) == sizeof(toRead)) {
		uint8_t* buffer = malloc(toRead.length);

		//check lseek
		lseek(fdat1, toRead.offset, SEEK_SET);

		//check read
		read(fdat1, buffer, toRead.length);

		if (*buffer < upperStart || *buffer > upperEnd) {
			continue;
		}


		lastStart += lastSize;

		//checks for write
		write(fdat2, buffer, toRead.length);
		write(fidx2, &lastStart, sizeof(uint16_t));
		write(fidx2, &toRead.length, sizeof(uint8_t));
		write(fidx2, &emptyByte, sizeof(uint8_t));

		lastSize = toRead.length;
	}

	close(fdat1);
	close(fidx1);
	close(fdat2);
	close(fidx2);

	exit(0);
}
