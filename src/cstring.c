#include "cstring.h"
#include <stdio.h>

void strAppend(char **str, const char *st) {
    int length = (strlen(*str) + strlen(st)) + 1;
    *str = (char*)realloc(*str, sizeof(char) * length);
    strcat(*str, st);
}

void strInsert(char **str, int pos, int len, char c) {
    int size = strlen(*str) + len + 1;
    char *new_str = (char*)malloc(sizeof(char) * size);
    strncpy(new_str, *str, pos - 1);
    
    int i = pos - 1;
    int j = 0; 
    while (j < len) {
        *(new_str + i) = c;
        j++;
        i++;
    }
    *(new_str + i) = '\0';
    
    strcat(new_str, (*str + (pos - 1)));
    free(*str);
    *str = new_str;
}

void strReplace(char **str, int pos, int len, const char *rep) {
    int size = (strlen(*str) - len) + strlen(rep) + 1;
    char *new = (char*)malloc(sizeof(char) * size);
    int i = pos - 1;
    strncpy(new, *str, i);
    strcat(new, rep);
    i += len;
    strcat(new, *str + i);

    free(*str);
    *str = new;
}

void strErase(char **str, int pos, int len) {
    int size = (strlen(*str) - len) + 1;
    char *new_array = (char *)malloc(sizeof(char) * size);
    
    int n = pos - 1;
    strncpy(new_array, *str, n);
    *(new_array + n) = '\0';
    n += len;
    strcat(new_array, *str + n);
    free(*str);
    *str = new_array;
}

char* strSubstr(char *str, int pos, int len) {
    char *psubstr = (str + (pos - 1));
    char *substr = (char *)malloc(sizeof(char) * (len + 1));
    
    strncpy(substr, psubstr, len);
    *(substr + len) = '\0';
    
    return substr; 
}

int strFind(char *str, const char *substring) {
    char *ret;
    ret = strstr(str, substring);
    if (ret != NULL)
        return (ret - str) + 1; 
    return -1;
}

void strRemSubstr(char **line, const char *str, const char *rep) {
    int tab = strFind(*line, str);
    if (tab == -1) {
        return;
    }
    else {
        strReplace(line, tab, 1, rep);
        strRemSubstr(line, str, rep);
    }
}