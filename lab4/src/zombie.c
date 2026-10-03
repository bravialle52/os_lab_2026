#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
  pid_t child_pid = fork();

  if (child_pid > 0) {
    printf("Родительский процесс PID: %d\n", getpid());
    printf("Дочерний процесс PID: %d (скоро станет зомби)\n", child_pid);
    printf("Введи `ps -l` в другом терминале, чтобы увидеть зомби (статус Z).\n");
    sleep(60); // Родитель спит и не вызывает wait()
  } else if (child_pid == 0) {
    exit(0); // Дочерний процесс сразу умирает
  } else {
    perror("fork failed");
    return 1;
  }

  return 0;
}
