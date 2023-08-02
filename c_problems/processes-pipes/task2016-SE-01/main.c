#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void closeSafe(int fd); 

void closeSafe(int fd) {
	int temp = errno;

	close(fd);

	errno = temp;
}

int main(int argc, char* argv[]) {
	
	if (argc != 2) {
		errx(1, "Wrong usage");
	}

	int a[2];

	if (pipe(a) == -1) {
		err(2, "Pipe failed");
	}

	const pid_t child = fork();

	if (child == -1) {
		err(3, "Fork failed");
	}

	if (child == 0) {
		
		close(a[0]);

		if (dup2(a[1], 1) == -1) {
			closeSafe(a[1]);
			err(4, "Could not dup2");
		}

		if (execlp("cat", "cat", argv[1], (char*)NULL) == -1) {
			err(5, "Cat execlp failed");
		}
	}

	close(a[1]);

	int status;

	if (wait(&status) == -1) {
		closeSafe(a[0]);
		err(6, "Could not wait for child process to finish");
	}	

	if (!WIFEXITED(status)) {
		closeSafe(a[0]);
		err(7, "Child process did not terminate properly");
	}

	if (WEXITSTATUS(status) != 0) {
		closeSafe(a[0]);
		err(8, "Child process did not exit with status 0");
	}

	if (dup2(a[0], 0) == -1) {
		closeSafe(a[0]);
		err(4, "Could not dup2");
	}

	if (execlp("sort", "sort", (char*)NULL) == -1) {
		err(9, "Sort execlp failed");
	}

	exit(0);
}
