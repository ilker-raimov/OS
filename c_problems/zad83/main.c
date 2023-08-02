#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

	if (argc > 2) {
		errx(1, "Wrong usage");
	}

	char* command;
	if (argc == 1) {
		command = (char*)"echo";
	} else {
		command = argv[1];
	}

	char c;
	char buff[5];
	uint8_t noRead = 0;
	uint32_t counter;

	while (1) {
	
		counter = 0;

		while (1) {

			if (read(0, &c, 1) != 1) {
				noRead = 1;

				break;
			}

			if (c == ' ' || c == '\n') {
				break;
			}

			if (counter == 4) {
				errx(2, "Too long");
			}
			
			buff[counter] = c;

			counter++;
		}

		if (noRead == 1) {
			break;
		}

		buff[counter] = '\0';

		const pid_t child = fork();

		if (child == -1) {
			err(3, "Could not fork");
		}

		if (child == 0) {
			if (execlp(command, command, buff, (char*)NULL) == -1) {
				err(4, "Could not execlp with %s", command);
			}
		}

		int status;

		if (wait(&status) == -1) {
			err(5, "Could not wait for child to finish");
		}

		if (!WIFEXITED(status)) {
			err(6, "Child did not terminate properly");
		}

		if (WEXITSTATUS(status) != 0) {
			err(7, "Child did not exit with 0");
		}

		buff[0] = '\0';
	}

	exit(0);
}
