#include <err.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

const char* whitespace = " ";
const char* newline = "\n";

uint64_t getNum(char* arg);
void writeSafe(const char* arg, int fd);
void writeNumSafe(uint64_t number, int fd);

uint64_t getNum(char* arg) {
	
	if (strlen(arg) != 1) {
		errx(2, "Time param is not the right size");
	}

	return arg[0] - '0';
}

void writeSafe(const char* arg, int fd) {
	if (write(fd, arg, strlen(arg)) != (ssize_t)strlen(arg)) {
		err(7, "Error writing");
	}
}

void writeNumSafe(uint64_t number, int fd) {
	char* toWrite = malloc(20 * sizeof(char));

	sprintf(toWrite, "%ld", number);

	if (write(fd, toWrite, strlen(toWrite)) != (ssize_t)strlen(toWrite)) {
		err(7, "Error writing");
	}

	free(toWrite);
}

int main(int argc, char* argv[]) {
	
	if (argc < 3) {
		errx(1, "Wrong usage");
	}

	char* command = argv[2];
	uint64_t timeArg = getNum(argv[1]);
	int commandArgsCount = argc - 3;
	char** commandArgs = (char**)NULL;


	if (commandArgsCount > 0) {
		commandArgs = malloc(commandArgsCount * sizeof(char*));

		for (int i = 3; i < argc; i++) {
			commandArgs[i - 3] = argv[i];
		}
	}

	int fd = open("run.log", O_CREAT | O_TRUNC | O_WRONLY, 0666);

	if (fd == -1) {
		err(6, "Could not open run.log");
	}

	uint8_t doNextLoop = 1;
	int currExitCode;
	int lastExitCode = 0;
	uint64_t startTime;
	uint64_t endTime;
	uint64_t currTimeDiff;
	uint64_t lastTimeDiff = timeArg + 1;

	while (doNextLoop) {
		
		const pid_t child = fork();

		if (child == -1) {
			err(3, "Could not fork");
		}

		startTime = time(NULL);

		if (child == 0) {

			if (commandArgs == (char**)NULL) {
				if (execl(command, command, (char*)NULL) == -1) {
					err(4, "Could not execv with command: %s", command);
				}
			} else {
				if (execv(command, commandArgs) == -1) {
					err(4, "Could not execv with command: %s", command);
				}
			}
		}

		int status;

		if (wait(&status) == -1) {
			err(5, "Could not wait for child to finish");
		}

		endTime = time(NULL);

		if (!WIFEXITED(status)) {
			currExitCode = 129;
		} else {
			currExitCode = WEXITSTATUS(status);
		}

		currTimeDiff = endTime - startTime;

		if (currExitCode != 0 && lastExitCode != 0 && lastTimeDiff < timeArg && currTimeDiff < timeArg) {
			doNextLoop = 0;
		}

		writeNumSafe(startTime, fd);
		writeSafe(whitespace, fd);
		writeNumSafe(endTime, fd);
		writeSafe(whitespace, fd);
		writeNumSafe((uint64_t)currExitCode, fd);
		writeSafe(newline, fd);

		lastExitCode = currExitCode;
		lastTimeDiff = currTimeDiff;
	}

	exit(0);
}
