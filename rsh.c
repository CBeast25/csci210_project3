#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

extern char **environ;

char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};

int isAllowed(const char*cmd) {
	for (int i = 0; i < 12; i++) {
        if (strcmp(cmd, allowed[i]) == 0)
            return 1;
    }
    return 0;
}

void printHelp() {
    printf("The allowed commands are:\n");
    for (int i = 0; i < N; i++) {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int main() {
	char line[256];

    while (1) {
		fprintf(stderr,"rsh>");

		if (fgets(line,256,stdin)==NULL) continue;
	
		if (strcmp(line,"\n")==0) continue;
	
		line[strlen(line)-1]='\0';

        char *argv[21];
        int argc = 0;
        char *token = strtok(line, " ");

        while (token != NULL && argc < 20) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        if (argc == 0) continue;

        char *cmd = argv[0];

        if (!isAllowed(cmd)) {
            printf("NOT ALLOWED!\n");
            continue;
        }

        if (strcmp(cmd, "exit") == 0) {
            return 0;
        } else if (strcmp(cmd, "help") == 0) {
            printHelp();
            continue;
        } else if (strcmp(cmd, "cd") == 0) {
            if (argc > 2) {
                printf("-rsh: cd: too many arguments\n");
            } else {
                const char *dir;
				if (argc == 2) {
					dir = argv[1];
				} else {
					dir = getenv("HOME");
				}
                if (chdir(dir) != 0) {
                    perror("cd failed");
                }
            }
            continue;
        }

        pid_t pid;
        int status;
        posix_spawnattr_t attr;

        posix_spawnattr_init(&attr);
        if (posix_spawnp(&pid, cmd, NULL, &attr, argv, environ) != 0) {
            perror("spawn failed");
            posix_spawnattr_destroy(&attr);
            continue;
        }

        waitpid(pid, &status, 0);
        posix_spawnattr_destroy(&attr);
    }

    return 0;
}
