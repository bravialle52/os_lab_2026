#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: %s seed array_size\n", argv[0]);
    return 1;
  }

  pid_t child_pid = fork();

  if (child_pid == -1) {
    perror("fork");
    return 1;
  }

  if (child_pid == 0) {
    printf("Child process (PID=%d) starting sequential_min_max...\n", getpid());
    execl("./sequential_min_max", "sequential_min_max", argv[1], argv[2], NULL);
    perror("execl");
    return 1;
  } else {
    printf("Parent process (PID=%d) waiting for child (PID=%d)...\n", getpid(), child_pid);
    int status;
    waitpid(child_pid, &status, 0);
    if (WIFEXITED(status)) {
      printf("Child process exited with code %d\n", WEXITSTATUS(status));
    } else {
      printf("Child process terminated abnormally\n");
    }
  }

  return 0;
}
