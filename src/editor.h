#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "buffer.h"
#include "cstring.h"

typedef struct {
    int x, y, px, py, rows, cols;
    char mode;
    Buffer *buff;
    WINDOW *pad, *padstatus;
    struct winsize win;
    const char *status, *filename;
}Editor;

Editor* edNew();
Editor* edNewStr(const char* filename);

char* ctos(Editor *editor, int n);
void edUpdateWindow(Editor *editor);
void edSaveFile(Editor *editor);

void edHandleInput(Editor *editor, int c);
void edPrintBuff(Editor *editor);
void edPrintStatusLine(Editor *editor);
void edUpdateStatus(Editor *editor);

void edMoveUp(Editor *editor);
void edMoveDown(Editor *editor);
void edMoveLeft(Editor *editor);
void edMoveRight(Editor *editor);