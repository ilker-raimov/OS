#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

const uint32_t MAX_FILE_NO_SPLIT = 3; //here is 50 mil

void sort(uint32_t* arr, const uint32_t size);

void sort(uint32_t* arr, const uint32_t size) {
	uint32_t minIndex;

	for (uint32_t i = 0; i + 1 < size; i++) {

		minIndex = i;

		for (uint32_t j = i + 1; j < size; j++) {
			
			if (arr[j] < arr[minIndex]) {
				minIndex = j;
			}
		}

		uint32_t temp = arr[i];
		arr[i] = arr[minIndex];
		arr[minIndex] = temp;
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		errx(1, "Wrong usage");
	}

	int input = open(argv[1], O_RDONLY);

	if (input == -1) {
		err(1, "Error opening input file");
	}

	off_t size = lseek(input, 0, SEEK_END);

	if (size == -1) {
		err(2, "Error seeking");
	}

	lseek(input, 0, SEEK_SET);

	uint32_t count = size / sizeof(uint32_t);

	if (count < MAX_FILE_NO_SPLIT) {
		uint32_t* arr = malloc(size);

		if (read(input, arr, size) != size) {
			err(3, "Error reading");
		}

		close(input);

		sort(arr, count);

		int output = open(argv[1], O_TRUNC | O_WRONLY);

		if (write(output, arr, size) != size) {
			err(4, "Error writing");
		}

		close(output);
	} else {
		uint32_t count1 = count / 2;
		uint32_t count2 = count - count1;
		uint32_t size1 = count1 * sizeof(uint32_t);
		uint32_t size2 = count2 * sizeof(uint32_t);
		uint32_t* arr1 = malloc(size1);

		if (read(input, arr1, size1) != size1) {
			err(3, "Error reading");
		}

		sort(arr1, count1);

		int temp1 = open("temp1", O_CREAT | O_TRUNC | O_RDWR, 0666);

		if (write(temp1, arr1, size1) != size1) {
			err(4, "Error writing");
		}

		free(arr1);

		uint32_t* arr2 = malloc(size2);

		if (read(input, arr2, size2) != size2) {
			err(3, "Error reading");
		}

		close(input);
		sort(arr2, count2);
		lseek(temp1, 0, SEEK_SET);
		//lseek check

		int output = open(argv[1], O_TRUNC | O_WRONLY);

		uint32_t first;
		uint32_t counter1 = 0;
		uint32_t counter2 = 0;

		read(temp1, &first, sizeof(uint32_t));

		for (; counter2 < count2 && counter1 < count1;) {
		
			if (first < arr2[counter2]) {
				write(output, &first, sizeof(uint32_t));
				read(temp1, &first, sizeof(uint32_t));
				counter1++;

				printf("fist if: %d\n", first);
			} else {
				write(output, &arr2[counter2], sizeof(uint32_t));
				counter2++;

				printf("second if: %d\n", arr2[counter2]);
			}
		}

		if (counter1 < count1) {
			
			for (; counter1 < count; counter1++) {
				read(temp1, &first, sizeof(uint32_t));
				write(output, &first, sizeof(uint32_t));

				printf("fist second chance: %d\n", first);
			}
		}

		if (counter2 < count2) {
			for (; counter2 < count2; counter2++) {
				write(output, &arr2[counter2], sizeof(uint32_t));

				printf("second second chance: %d\n", arr2[counter2]);
			}
		}

		free(arr2);
		close(output);
	}

	exit(0);
}
