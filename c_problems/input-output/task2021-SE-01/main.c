#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>


int main(int argc, char* argv[]) {

	if (argc != 3) {
		errx(1, "Wrong usage");
	}

	int input = open(argv[1], O_RDONLY);
	int output = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);

	//fd checks

	uint8_t inputRead;
	uint8_t outputTemp1;
	uint8_t outputTemp2;

	while(read(input, &inputRead, sizeof(uint8_t)) == sizeof(uint8_t)) {
		
		outputTemp1 = 0;
		outputTemp2 = 0;

		for(uint8_t bitShift = 0; bitShift < 4; bitShift++) {
			
			uint8_t mask = 128 >> bitShift;

			outputTemp1 = outputTemp1 << 2;	

			if ((inputRead & mask) == 0) {
				
				outputTemp1 = outputTemp1 | 1;
			} else {
				
				outputTemp1 = outputTemp1 | 2;
			}
		}

		for(uint8_t bitShift = 4; bitShift < 8; bitShift++) {
			
			uint8_t mask = 128 >> bitShift;

			outputTemp2 = outputTemp2 << 2;	

			if ((inputRead & mask) == 0) {
				
				outputTemp2 = outputTemp2 | 1;
			} else {
				
				outputTemp2 = outputTemp2 | 2;
			}
		}

		write(output, &outputTemp1, sizeof(uint8_t));
		write(output, &outputTemp2, sizeof(uint8_t));
	}

	close(input);
	close(output);

	exit(0);
}
