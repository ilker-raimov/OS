#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>

typedef struct {
	uint32_t x;
	uint32_t y;
} pair;

int main(int argc, char* argv[]) {

	if (argc != 3) {
		errx(1, "Wrong usage");
	}

	int fd1 = open(argv[1], O_RDONLY);

	if (fd1 == -1) {
		err(2, "Error opening %s", argv[1]);
	}

	int fd2 = open(argv[2], O_RDONLY);

	if (fd2 == -1) {
		err(2, "Error opening %s", argv[2]);
	}

	int fd3 = open("f3", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);

	if (fd3 == -1) {
		err(2, "Error opening %s", "f3");
	}

	ssize_t read_size;
	pair toRead;

	while((read_size = read(fd1, &toRead, sizeof(pair))) == sizeof(pair)) {
		uint32_t offset = (toRead.x - 1) * sizeof(uint32_t);
		uint32_t length = toRead.y * sizeof(uint32_t);

		if (lseek(fd2, offset, SEEK_SET) == -1) {
			err(4,"Error seeking");
		}

		uint32_t* buffer = malloc(length);

		if (read(fd2, buffer, length) != length) {
			err(3, "Error reading");
		}

		if (write(fd3, buffer, length) != length) {
			err(4, "Error writing");
		}

		free(buffer);
	}

	//if buffer != nullptr - free()
	close(fd1);
	close(fd2);
	close(fd3);

	exit(0);
}
