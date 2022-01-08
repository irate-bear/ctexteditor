#include <string.h>
#include <stdlib.h>

void strAppend(char **str, const char *st);
void strInsert(char **str, int pos, int len, char c);
void strReplace(char **str, int pos, int len, const char *rep);
void strErase(char **str, int pos, int len);
char* strSubstr(char *str, int pos, int len);
int strFind(char *str, const char *st);
void strRemSubstr(char **line, const char *str, const char *rep);