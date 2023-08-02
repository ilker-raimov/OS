#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void checkStatus(void);

void checkStatus(void) {
	int status;

	if (wait(&status) == -1) {
		err(6, "Could not wait for child to finish");
	}

	if (!WIFEXITED(status)) {
		err(7, "Child did not terminate properly");
	}

	if (WEXITSTATUS(status) != 0) {
		err(8, "Child did not exit with 0");
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		errx(1, "Wrong usage");
	}

	int p1[2];

	if (pipe(p1) == -1) {
		err(2, "Could not pipe");
	}

	const pid_t child1 = fork();

	if (child1 == -1) {
		err(3, "Could not fork");
	}

	if (child1 == 0) {
		close(p1[0]);

		if (dup2(p1[1], 1) == -1) {
			err(4, "Could not dup");
		}

		if (execlp("find", "find", argv[1], "-type", "f", "-printf", "%T@ %P\n", (char*)NULL) == -1) {
			err(5, "Could not execlp");
		}
	} else {
		close(p1[1]);
	}

	checkStatus();

	int p2[2];

	if (pipe(p2) == -1) {
		err(2, "Could not pipe");
	}

	const pid_t child2 = fork();

	if (child2 == -1) {
		err(3, "Could not fork");
	}

	if (child2 == 0) {
		close(p2[0]);

		if (dup2(p2[1], 1) == -1 || dup2(p1[0], 0) == -1) {
			err(4, "Could not dup");
		}

		if (execlp("sort", "sort", "-nr", "-k1", (char*)NULL) == -1) {
			err(5, "Could not execlp");
		}
	} else {
		close(p2[1]);
	}

	checkStatus();

	if (dup2(p2[0], 0) == -1) {
		err(4, "Could not dup");
	}

	if (execlp("head", "head", "-n", "1", (char*)NULL) == -1) {
		err(5, "Could not execlp");
	}

	exit(0);
}
