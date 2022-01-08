#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_MIN_CAPACITY 10

typedef struct {
    char **lines;
    int size;
    int capacity;
}Buffer;

Buffer* bufNew();
void  bufDestroy(Buffer *buffer);

void bufInsertLine(Buffer *buffer, char *line, int linenum);
void bufAppendLine(Buffer *buffer, char *line);
void bufRemoveLine(Buffer *buffer, int pos);
void bufSplitLine(Buffer *buffer, int linenum, int pos);
void bufReplaceLine(Buffer *buffer, char *line, int linenum);
