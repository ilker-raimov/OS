#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/wait.h>

int main(void) {

	int p1[2];

	if (pipe(p1) == -1) {
		err(1, "Could not pipe");
	}

	const pid_t child1 = fork();

	if (child1 == -1) {
		err(2, "Could not fork");
	}

	if (child1 == 0) {

		close(p1[0]);

		if (dup2(p1[1], 1) == -1) {
			err(3, "Could not dup");
		}

		if (execlp("cut", "cut", "-d", ":", "-f", "7", "/etc/passwd", (char*)NULL) == -1) {
			err(4, "Could not execlp");
		}
	} else {
		close(p1[1]);
	}
	

	int status;

	if (wait(&status) == -1) {
		err(5, "Could not wait for child to finish");
	}

	if (!WIFEXITED(status)) {
		err(6, "Child did not terminate properly");
	}

	if (WEXITSTATUS(status) != 0) {
		err(7, "Child exited with status not 0");
	}

	int p2[2];

	if (pipe(p2) == -1) {
		err(1, "Could not pipe");
	}

	const pid_t child2 = fork();

	if (child2 == -1) {
		err(2, "Could not fork");
	}

	if (child2 == 0) {

		close(p2[0]);

		if (dup2(p1[0], 0) == -1 || dup2(p2[1], 1) == -1) {
			err(3, "Could not dup");
		}

		if (execlp("sort", "sort", (char*)NULL) == -1) {
			err(4, "Could not execlp");
		}
	} else {
		close(p2[1]);
	}

	int status2;

	if (wait(&status2) == -1) {
		err(5, "Could not wait for child to finish");
	}

	if (!WIFEXITED(status2)) {
		err(6, "Child did not terminate properly");
	}

	if (WEXITSTATUS(status2) != 0) {
		err(7, "Child exited with status not 0");
	}

	int p3[2];

	if (pipe(p3) == -1) {
		err(1, "Could not pipe");
	}

	const pid_t child3 = fork();

	if (child3 == -1) {
		err(2, "Could not fork");
	}

	if (child3 == 0) {

		close(p3[0]);

		if (dup2(p2[0], 0) == -1 || dup2(p3[1], 1) == -1) {
			err(3, "Could not dup");
		}

		if (execlp("uniq", "uniq", "-c", (char*)NULL) == -1) {
			err(4, "Could not execlp");
		}
	} else {
		close(p3[1]);
	}

	int status3;

	if (wait(&status3) == -1) {
		err(5, "Could not wait for child to finish");
	}

	if (!WIFEXITED(status3)) {
		err(6, "Child did not terminate properly");
	}

	if (WEXITSTATUS(status3) != 0) {
		err(7, "Child exited with status not 0");
	}

	dup2(p3[0], 0);

	execlp("sort", "sort", "-n", "-k1", (char*)NULL);

	exit(0);
}
