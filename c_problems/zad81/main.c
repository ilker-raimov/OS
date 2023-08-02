#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdio.h>

const uint16_t buffSize = 4096;
const char* exitMsg = "exit";
const char* firstLine = "Enter command:\n";

int main(void) {
	
	ssize_t bytesRead;
	char buff[buffSize];

	if (write(1, firstLine, strlen(firstLine)) != (ssize_t)strlen(firstLine)) {
		err(1, "Error writing");
	}

	while ((bytesRead = read(0, buff, buffSize)) > 0) {
		
		buff[bytesRead - 1] = '\0';

		if (strcmp(buff, exitMsg) == 0) {
			exit(0);
		}

		const pid_t child = fork();

		if (child == -1) {
			err(2, "Could not fork");
		}

		if (child == 0) {
			
			char command[buffSize];

			strcpy(command, "/bin/");
			strcat(command, buff);


			if (execl(command, buff, (char*)NULL) == -1) {
				err(3, "Could not execl %s", command);
			}
		}

		int status;

		if (wait(&status) == -1) {
			err(4, "Could not wait for child process to finish");
		}

		if (!WIFEXITED(status)) {
			err(5, "Child proccess could not terminate properly");
		}

		if (WEXITSTATUS(status) != 0) {
			err(6, "Child proccess did not exit with 0");
		}

		if (write(1, firstLine, strlen(firstLine)) != (ssize_t)strlen(firstLine)) {
			err(1, "Error writing");
		}
	}

	if (bytesRead != 0) {
		err(7, "Error reading");
	}

	exit(0);
}
