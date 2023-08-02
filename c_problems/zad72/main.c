#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

	if (argc < 1) {
		err(1, "hehe");
	}

	if (!argv) {
		err(1, "hehe2");
	}

	int fd = open(argv[1], O_RDONLY);

	uint16_t* buff = malloc(16 * sizeof(uint8_t));

	read(fd, buff, 16 * sizeof(uint8_t));

	uint16_t* ptr = buff;

	printf("%d\n", *(ptr + 4));
	printf("%d\n", *(ptr + 5));

	exit(0);
}
