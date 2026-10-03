#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> // Добавлено для сигналов

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

// Глобальные переменные для доступа из обработчика сигнала
pid_t *child_pids;
int active_child_processes = 0;

// Обработчик сигнала SIGALRM (срабатывает по таймауту)
void handle_alarm(int sig) {
    printf("\nTimeout reached! Killing child processes...\n");
    for (int i = 0; i < active_child_processes; i++) {
        kill(child_pids[i], SIGKILL);
    }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1; // Новая переменная для таймаута
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0}, // Добавлена новая опция
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // error handling
            break;
          case 3:
            timeout = atoi(optarg); // Обработка аргумента timeout
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  
  // Выделяем память под массив PID дочерних процессов
  child_pids = malloc(sizeof(pid_t) * pnum);
  active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      child_pids[active_child_processes] = child_pid; // Сохраняем PID
      active_child_processes += 1;
      
      if (child_pid == 0) {
        // child process
        // parallel somehow

        if (with_files) {
          // use files here
        } else {
          // use pipe here
        }
        return 0; // Дочерний процесс должен завершиться
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  // Если был передан --timeout, запускаем таймер
  if (timeout > 0) {
      signal(SIGALRM, handle_alarm);
      alarm(timeout);
  }

  // Цикл неблокирующего ожидания с использованием WNOHANG
  while (active_child_processes > 0) {
    int status;
    pid_t wpid = waitpid(-1, &status, WNOHANG);
    
    if (wpid > 0) {
        active_child_processes -= 1; // Процесс успешно завершился
    } else if (wpid == 0) {
        // Процессы всё еще работают, немного подождем, чтобы не нагружать процессор
        usleep(10000); // 10 миллисекунд
    } else {
        // Ошибка (например, больше нет процессов)
        break;
    }
  }

  // Отключаем таймер, если все дети отработали быстрее, чем истек таймаут
  if (timeout > 0) {
      alarm(0);
  }

  free(child_pids); // Очищаем память

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
    } else {
      // read from pipes
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
