#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

const char* pipeName = "./pipe69v2.txt";

int main(int argc, char* argv[]) {
	
	if (argc != 2) {
		errx(1, "Wrong usage");
	}

	if (mkfifo(pipeName, 0777) == -1) {
		err(2, "Could not mkfifo");
	}

	const pid_t child = fork();

	if (child == -1) {
		err(3, "Could not fork");
	}

	if (child == 0) {
		
		int np = open(pipeName, O_WRONLY);

		if (np == -1) {
			err(4, "Could not open %s", pipeName);
		}

		if (dup2(np, 1) == -1) {
			err(5, "Could not dup");
		}

		if (execlp("cat", "cat", argv[1], (char*)NULL) == -1) {
			err(6, "Could not execlp");
		}
	}

	int status;

	if (wait(&status) == -1) {
		err(7, "Could not wait for child to finish");
	}

	if (!WIFEXITED(status)) {
		err(8, "Child did not terminate properly");
	}

	if (WEXITSTATUS(status) != 0) {
		err(9, "Child did not exit with status 0");
	}

	exit(0);
}
