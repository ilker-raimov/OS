#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>

void selectionSort(uint16_t* arr, uint32_t size);

void selectionSort(uint16_t* arr, uint32_t size) {
	uint32_t minIndex;

	for (uint32_t i = 0; i + 1 < size; i++) {
		
		minIndex = i;

		for (uint32_t j = i + 1; j < size; j++) {
			if (arr[j] < arr[minIndex]) {
				minIndex = j;
			}
		}

		uint16_t temp = arr[i];

		arr[i] = arr[minIndex];
		arr[minIndex] = temp;
	}
}

int main(int argc, char* argv[]) {
	
	if (argc != 3) {
		errx(1, "Wrong usage");
	}

	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0600);

	//fd checks for skumies

	ssize_t size = lseek(fd1, 0, SEEK_END);

	lseek(fd1, 0, SEEK_SET);

	uint32_t numCount = size / sizeof(uint16_t);

	if (numCount > 65535) {
		errx(3, "Too many nums in file1");
	}

	uint16_t* arr = malloc(size);

	if (read(fd1, arr, size) != size) {
		err(2, "Error reading");
	}

	selectionSort(arr, numCount);

	if (write(fd2, arr, size) != size) {
		err(4, "Error writing to file2");
	}

	exit(0);
}
