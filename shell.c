
// Kuira Edwards @02942519

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[256] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
int alarmFlag = 0;
int alarms = 0;
int kill_pidt;
int child_pid;

time_t start, stop;

void alarmhandler(int signum) { //handles alarms
    kill(child_pid, SIGKILL);
}

void signalhandler(int signum) { //handles signals
    signal(SIGINT, signalhandler);
    fflush(stdout);
}

int main() {
    int i = 0;
    int echo_int = 1;
    int env_int = 1;
    bool is_background = false;
    char *text;
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    size_t n = sizeof(arguments) / sizeof(arguments[0]);

    signal(SIGINT, signalhandler);
    signal(SIGALRM, alarmhandler) ;

    while (true) {
      
        do{
            int bufsize = 256;

            getcwd(prompt, bufsize); 
            // Print the shell prompt.
            printf("%s", prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        } while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 
        
			  // 0. Modify the prompt to print the current working directory
			  
			
        // 1. Tokenize the command line input (split it on whitespace)
        arguments[0] = strtok(command_line, delimiters);

        i = 0;
        while(arguments[i] != NULL) {
            i++;
            arguments[i] = strtok(NULL, delimiters);
        }

        if(strcmp(arguments[i-1], "&")== 0) {
            is_background = true;
            arguments[i-1] = '\0';
        }

        // 2. Implement Built-In Commands
        if(strcmp(arguments[0], "exit") == 0) {
            exit(1);
        } else if(strcmp(arguments[0], "pwd") == 0) {
            printf("%s\n", prompt);
        } else if(strcmp(arguments[0], "cd") == 0) {
            if(chdir(arguments[1]) != 0) {
                printf("change directory to %s failed", arguments[1]);
            } else {
                if(arguments[1] == NULL) {
                    chdir(getenv("HOME"));
                } else {
                    chdir(arguments[1]);
                }
            }
        } else if (strcmp(arguments[0], "env") == 0) {
            env_int = 1;

            printf("PATH : %s\n", getenv("PATH"));
            printf("HOME : %s\n", getenv("HOME"));
            printf("ROOT : %s\n", getenv("ROOT"));

            while(arguments[env_int] != NULL) {
                text = arguments[env_int];
                if (strstr(text, "$") != NULL) {
                    text += 1;
                }
            printf("%s : ", text);
            printf("%s\n", getenv(text));
            env_int++;
            }
        }
        else if (strcmp(arguments[0], "echo") == 0) {
            echo_int = 1;
            while (arguments[echo_int] != NULL) {
                text = arguments[echo_int];
                if(strstr(text, "$") != NULL) {
                    text +=1;
                    text = getenv(text);
                    printf("%s \n", text);
                }
                else{
                  printf("%s ", arguments[echo_int]);
                  }
                  echo_int++;
            }
              printf("\n");
        } else if (strcmp(arguments[0], "setenv") == 0) {
    if (arguments[1] != NULL && arguments[2] != NULL) {
        setenv(arguments[1], arguments[2], 1);
    }
    else {
        printf("ERROR\n");
    }
} else {
        // 3. Create a child process which will execute the command line input
        pid_t cpid;
        int status;
        cpid = fork();

        if(cpid < 0) {
            printf("ERROR\n");
        }
        else if(cpid==0) {
            if(execvp(arguments[0], arguments) == -1) {
                perror("execvp() fail: ");
                printf("Error\n");
            }
            exit(0);
        }
        else {
            child_pid = cpid;
            alarm(10);
            if(!is_background) {
                wait(NULL);
            }
        }
}
        // 4. The parent process should wait for the child to complete unless its a background process
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}