#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

typedef struct {
	char filename[8];
	uint32_t offset;
	uint32_t length;
} triplet;

int main(int argc, char** argv) {
	
	if (argc != 2) {
		errx(1, "Wrong usage");
	}

	struct stat st;

	if (stat(argv[1], &st) == -1) {
		err(2, "Could not stat %s", argv[1]);
	}

	if (st.st_size == 0) {
		errx(3, "%s is empty, nothing to do", argv[1]);
	}

	if (st.st_size % sizeof(triplet) != 0 || (size_t)st.st_size > sizeof(triplet) * 8) {
		errx(4, "%s has invalid content", argv[1]);
	}

	int fdmain = open(argv[1], O_RDONLY);

	if (fdmain == -1) {
		err(8, "Could not open %s", argv[1]);
	}

	uint8_t tripletCount = st.st_size / sizeof(triplet);
	int fd[tripletCount];
	int p[tripletCount][2];

	for (uint8_t i = 0; i < tripletCount; i++) {

		triplet currentTriplet;
		const pid_t child = fork();	

		if (read(fdmain, &currentTriplet, sizeof(triplet)) != (ssize_t)sizeof(triplet)) {
			err(7, "Could not read1");
		}

		if (child == -1) {
			err(5, "Could not fork");	
		}

		if (pipe(p[i]) == -1) {
			err(6, "Could not pipe");
		}

		if (child == 0) {
			close(p[i][0]);

			printf("filename: %s\n", currentTriplet.filename);

			fd[i] = open(currentTriplet.filename, O_RDONLY);

			if (fd[i] == -1) {
				err(8, "Could not open %s", currentTriplet.filename);
			}

			if (lseek(fd[i], currentTriplet.offset * sizeof(uint16_t), SEEK_SET) == -1) {
				err(9, "Could not seek");
			}

			uint16_t toRead;
			uint16_t xor = 0x0000;

			for (uint32_t j = 0; j < currentTriplet.length; j++) {
				if (read(fd[i], &toRead, sizeof(uint16_t)) != (ssize_t)sizeof(uint16_t)) {
					err(7, "Could not read2");
				}

				xor ^= toRead;
			}

			close(fd[i]);

			if (write(p[i][1], &xor, sizeof(uint16_t)) != (ssize_t)sizeof(uint16_t)) {
				err(10, "Could not write");
			}

			exit(0);
		} else {
			close(p[i][1]);
		}
	}

	uint16_t currXor;
	uint16_t finalXor = 0x0000;

	for (uint8_t i = 0; i < tripletCount; i++) {
		if (read(p[i][0], &currXor, sizeof(uint16_t)) != (ssize_t)sizeof(uint16_t))	{
			err(7, "Could not read3");
		}

		close(p[i][0]);
		finalXor ^= currXor;
	}

	printf("result: %dB\n", finalXor);

	close(fdmain);

	exit(0);
}
