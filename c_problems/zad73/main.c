#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

	if (argc != 3) {
		errx(1, "Wrong usage");
	}

	int scl = open(argv[1], O_RDONLY);
	int sdl = open(argv[2], O_RDONLY);
	int output = open("output", O_CREAT | O_TRUNC | O_WRONLY, 0666); 
	//fd checks

	off_t sclSize = lseek(scl, 0, SEEK_END);
	off_t sdlSize = lseek(sdl, 0, SEEK_END);

	lseek(scl, 0, SEEK_SET);
	lseek(sdl, 0, SEEK_SET);

	//lseek checks

	if (sclSize * 16 != sdlSize) {
		err(1, "File sizes do not match");
	}

	uint8_t sclRead;
	uint16_t sdlRead;
	off_t offset = 0;

	while (read(scl, &sclRead, sizeof(uint8_t)) == sizeof(uint8_t)) {
		
		uint8_t mask;

		for (uint8_t bitShift = 0; bitShift < 8; bitShift++, offset++) {

			mask = 128 >> bitShift;

			if ((sclRead & mask) > 0) {
				lseek(sdl, offset * sizeof(uint16_t), SEEK_SET);

				read(sdl, &sdlRead, sizeof(uint16_t));
				write(output, &sdlRead, sizeof(uint16_t));
			}
		}
	}

	exit(0);
}
