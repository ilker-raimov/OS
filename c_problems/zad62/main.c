#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

	if (argc != 4) {
		errx(1, "Wrong usage skum");
	}

	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_RDONLY);
	int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0666);

	//fd checks

	uint8_t byte1 = 0, byte2 = 0;
	uint8_t bytesRead1, bytesRead2;
	uint16_t offset = 0;

	while((bytesRead1 = read(fd1, &byte1, sizeof(uint8_t))) == sizeof(uint8_t) && (bytesRead2 = read(fd2, &byte2, sizeof(uint8_t))) == sizeof(uint8_t)) {
		if (byte1 != byte2) {
			write(fd3, &offset, sizeof(uint16_t));
			write(fd3, &byte1, sizeof(uint8_t));
			write(fd3, &byte2, sizeof(uint8_t));
		}

		printf("%d i %d\n", byte1 + 1000, byte2 + 1000);
		//write and read checks		

		offset++;
	}	

	close(fd1);
	close(fd2);
	close(fd3);

	exit(0);
}
