#include "revert_string.h"
#include <string.h>
#include <stdlib.h>

void RevertString(char *str)
{
    if (str == NULL) return;

    int len = strlen(str);
    // Выделяем память под временную копию строки в куче (+1 для '\0')
    char *temp = (char *)malloc((len + 1) * sizeof(char));

    strcpy(temp, str);

    // Переворачиваем строку, записывая символы с конца temp в начало str
    for (int i = 0; i < len; i++) {
        str[i] = temp[len - 1 - i];
    }

    // Освобождаем выделенную память, чтобы избежать утечек
    free(temp);
}
