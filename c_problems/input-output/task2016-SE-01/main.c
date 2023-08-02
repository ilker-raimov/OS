#include <err.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

void selectionSort(uint8_t* arr, size_t size);

void selectionSort(uint8_t* arr, size_t size) {
	
		size_t i, j, minIndex;

		for (i = 0; i + 1 < size; i++) {
			minIndex = i;

			for (j = i + 1; j < size; j++) {
				if (arr[j] < arr[minIndex]) {
					minIndex = j;
				}
			}

			write(1, "hihi\n", 5);
			uint8_t temp = arr[i];
	
			arr[i] = arr[minIndex];
			arr[minIndex] = temp;
		}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "Wrong argc");
	}

	int fd = open(argv[1], O_RDWR);

	if (fd == -1) {
		err(2, "Error opening %s", argv[1]);
	}

	//checks for seek
	size_t size = lseek(fd, 0, SEEK_END);

	lseek(fd, 0, SEEK_SET);
	
	uint8_t* bytes = malloc(size);

	if (read(fd, &bytes, size) != size) {
		err(3, "Error reading");
	}
	
	printf("%d", sizeof(bytes));
	close(fd);

	int fd2 = open("tempSorted", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);

	if (fd2 == -1) {
		err(2, "Error opening %s", "tempSorted");
	}
//mssing

	close(fd2);

	exit(0);
}
