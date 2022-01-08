#include "buffer.h"
#include "cstring.h"

void bufCopyArray(char **dest, char **src, int n) {
    int i = 0;
    while(*(src + i) != NULL && i < n) {
        *(dest + i) = strdup(*(src + i));
        i++;
    }
    *(dest + i) = NULL;
}

void bufDestroyArray(char **arr) {
    char **v = arr;
    while (*v != NULL) {
        char *temp = *v;
        v++;
        free(temp);
    }
    free(arr);
}

void bufResize(Buffer *buffer) {
    size_t cap = 2 * ((int)(buffer->capacity / 10) * 10) + 1;
    char **lines = (char**)malloc(sizeof(char*) * cap);
    if (!lines) {
        exit(EXIT_FAILURE);
    }
    else {
        bufCopyArray(lines, buffer->lines, buffer->size);

        char **temp = buffer->lines;
        buffer->lines = lines;
        buffer->capacity = cap - 1;
        bufDestroyArray(temp);
    }
}

void bufShrink(Buffer *buffer) {
    size_t cap = ((int)(buffer->capacity / 10) * 10) / 2;
    char **lines = (char**)malloc(sizeof(char*) * (cap + 1));
    if (!lines) {
        exit(EXIT_FAILURE);
    }
    else {
        bufCopyArray(lines, buffer->lines, buffer->size);

        char **temp = buffer->lines;
        buffer->lines = lines;
        buffer->capacity = cap;
        bufDestroyArray(temp);
    }
}

Buffer* bufNew() {
    int min_capacity = BUFFER_MIN_CAPACITY;
    Buffer *buffer = (Buffer*)malloc(sizeof(Buffer));
    char **lines = (char**)malloc(sizeof(char*) * (min_capacity + 1));
    *lines = NULL;
    buffer->lines = lines;
    buffer->capacity = BUFFER_MIN_CAPACITY;
    buffer->size = 0;
    return buffer;
}

void  bufDestroy(Buffer *buffer) {
    char **v = buffer->lines;
    while (*v != NULL) {
        char *temp = *v;
        v++;
        free(temp);
    }
    free(buffer->lines);
    free(buffer);
}

void bufSplitLine(Buffer *buffer, int linenum, int pos) {
    char **buf = buffer->lines;
    char *line = buf[linenum - 1];
    int len = strlen(line);
    int spllen = len - (pos - 1);
    char *split = strSubstr(line, pos, spllen);
    strErase(buf, pos, spllen);
    bufInsertLine(buffer, split, linenum + 1);
}

void bufInsertLine(Buffer* buffer, char *line, int linenum) {
    if (buffer->size == buffer->capacity) {
        bufResize(buffer);
    }

    strRemSubstr(&line, "\t", "    ");
    char **lines = buffer->lines;
    char **new_buffer = (char**)malloc(sizeof(char*) * (buffer->capacity + 1));
    
    int i = 0;

    while (i < (linenum - 1) && *lines != NULL) {
        *(new_buffer + i) = *lines++;
        i++;
    }

    *(new_buffer + i) = line;
    i++;
    while (*lines != NULL) {
        *(new_buffer + i) = *(lines++);
        i++;
    }
    *(new_buffer + i) = NULL;
    free(buffer->lines);
    buffer->lines = new_buffer;
    buffer->size++;
}

void bufAppendLine(Buffer* buffer, char *line) {
    if (buffer->size == buffer->capacity) {
        bufResize(buffer);
    }
    
    strRemSubstr(&line, "\t", "    ");
    *(buffer->lines + buffer->size) = line;
    buffer->size++;
    *(buffer->lines + buffer->size) = NULL;
}

void bufRemoveLine(Buffer* buffer, int pos) {
    char **lines = buffer->lines;

    if (pos > buffer->size) {
        return;
    }

    int i = 0;
    while (i < (pos - 1) && *lines != NULL) {
        i++;
    }
    
    if (*(lines + i) != NULL)
        free(*(lines + i)); 
    
    i++;

    while (*(lines + i) != NULL) {
        *(lines + (i - 1)) = *(lines + i);
        i++;
    }
    *(lines + (i - 1)) = NULL;
    buffer->size--;

    if ((buffer->size == (buffer->capacity) / 2) && buffer->capacity > 10) {
        bufShrink(buffer);
    }
} 

void bufReplaceLine(Buffer *buffer, char *line, int linenum) {
    free(buffer->lines[linenum - 1]);
    buffer->lines[linenum - 1] = line;
}