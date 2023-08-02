#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
	uint32_t uid;
	uint16_t useless1;
	uint16_t useless2;
	uint32_t timeStart;
	uint32_t timeEnd;
} user;

double getAvg(const user* users, const uint16_t size);
double getDispersion(const user* users, const uint16_t size, const double avg);
void sortByUid(user* users, const uint16_t size); 
void sortByDiff(user* users, const uint16_t size); 

double getAvg(const user* users, const uint16_t size) {
	uint64_t sum = 0;

	for (uint16_t i = 0; i < size; i++) {
		sum += users[i].timeEnd - users[i].timeStart;
	}

	return sum * 1.0 / size;
}

double getDispersion(const user* users, const uint16_t size, const double avg) {
	double diff;
	long double sum = 0;

	for (uint16_t i = 0; i < size; i++) {

		diff = (double)(users[i].timeEnd - users[i].timeStart) - avg;

		sum += diff * diff;
	}

	return sum / size;
}

void sortByUid(user* users, const uint16_t size) {

	if (size == 0) {

		return;
	}

	uint16_t minIndex;

	for (uint16_t i = 0; i + 1 < size; i++) {

		minIndex = i;

		for (uint16_t j = i + 1; j < size; j++) {

			if (users[j].uid < users[minIndex].uid) {

				minIndex = j;
			}
		}

		user temp = users[i];
		users[i] = users[minIndex];
		users[minIndex] = temp;
	}
}

void sortByDiff(user* users, const uint16_t size) {

	if (size == 0) {

		return;
	}

	uint16_t minIndex;
	uint16_t prevUidSwitchIndex = 0;
	uint32_t diff, minDiff;

	for (uint16_t i = 1; i < size; i++) {

		if (users[i].uid != users[i - 1].uid) {
			
			for (uint16_t j = prevUidSwitchIndex; j + 1 < i; j++) {

				minIndex = j;	
				minDiff = users[j].timeEnd - users[j].timeStart;

				for (uint16_t k = j + 1; k < i; k++) {
					diff = users[k].timeEnd - users[k].timeStart;

					if (diff < minDiff) {
	
						minIndex = k;
						minDiff = diff;
					}
				}

				user temp = users[j];
				users[j] = users[minIndex];
				users[minIndex] = temp;
			}

			prevUidSwitchIndex = i;
		}

		if (i + 1 == size) {
			
			for (uint16_t j = prevUidSwitchIndex; j + 1 <= i; j++) {

				minIndex = j;	
				minDiff = users[j].timeEnd - users[j].timeStart;

				for (uint16_t k = j + 1; k <= i; k++) {
					diff = users[k].timeEnd - users[k].timeStart;

					if (diff < minDiff) {
	
						minIndex = k;
						minDiff = diff;
					}
				}

				user temp = users[j];
				users[j] = users[minIndex];
				users[minIndex] = temp;
			}

			prevUidSwitchIndex = i;
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		errx(1, "Wr usage");
	}

	int fd1 = open(argv[1], O_RDONLY);

	//fd check

	ssize_t size = lseek(fd1, 0, SEEK_END);

	if (size == 0) {

		close(fd1);
		exit(0);
	}

	lseek(fd1, 0, SEEK_SET);

	//lseek checks

	if (size % sizeof(user) != 0) {
		errx(2, "File has invalid size!");
	}

	uint16_t usersCount = size / sizeof(user);

	user* users = malloc(size);

	if (read(fd1, users, size) != size) {
		err(3, "Error reading from file");
	}

	close(fd1);

	uint32_t diff;
	double sqrDiff;
	double avg = getAvg(users, usersCount);
	double dispersion = getDispersion(users, usersCount, avg);	

	sortByUid(users, usersCount);
	sortByDiff(users, usersCount);

	for (uint16_t i = 0; i + 1 < usersCount; i++) {
		
		if (users[i].uid != users[i + 1].uid) {

			diff = users[i].timeEnd - users[i].timeStart;
			sqrDiff = diff * 1.0 * diff;

			if (sqrDiff > dispersion) {
				printf("%d - ", users[i].uid);
				printf("%d\n", diff);
			}
		}

		printf("curr user uid: %d and his diff: %d\n", users[i].uid, users[i].timeEnd - users[i].timeStart);
	}

	diff = users[usersCount - 1].timeEnd - users[usersCount - 1].timeStart;
	sqrDiff = diff * 1.0 * diff;

	if (sqrDiff > dispersion) {
		printf("%d - ", users[usersCount - 1].uid);
		printf("%d\n", diff);
	}

	printf("curr user uid: %d and his diff: %d\n", users[usersCount - 1].uid, users[usersCount - 1].timeEnd - users[usersCount - 1].timeStart);
	printf("disp: %F\n", dispersion);
	
	exit(0);
}
