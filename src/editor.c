#include "editor.h"

#define EDITOR_MAX_COLS 255
#define EDITOR_MAX_ROWS 255

void edDelete(Editor *editor) {
    if ((editor->x == 0) && (editor->y > 0)) {
        strAppend(&(editor->buff->lines[editor->y - 1]), editor->buff->lines[editor->y]);
        editor->x = strlen(editor->buff->lines[editor->y - 1]);
        bufRemoveLine(editor->buff, editor->y + 1);
        editor->y--;
        
        if ((editor->y - editor->py) < 0) {
            editor->py--;
        }
        if ((editor->x - editor->px) < 0) {
            editor->px = editor->x - 1;
        }
    }
    else {
        if (editor->x > 0) {
            strErase(&(editor->buff->lines[editor->y]), editor->x, 1);
            editor->x--;
        }
    }
}

void edLineWrapStr(Editor *editor, int linenum, int n) {
    char *line = strdup(editor->buff->lines[linenum - 1]);
    int pos = n;
    int len = strlen(line) - pos + 1;
    char *str = strSubstr(line, pos, len);
    strErase(&(line), pos, len);
    bufReplaceLine(editor->buff, line, linenum);
    line = editor->buff->lines[linenum - 1];
    bufInsertLine(editor->buff, str, linenum + 1);
}

void edEnterReset(Editor *editor) {
    if ((editor->y < EDITOR_MAX_ROWS - 1) && (editor->buff->size  < EDITOR_MAX_ROWS - 1)) {
        edLineWrapStr(editor, editor->y + 1, editor->x + 1);
        editor->y++;
        if ((editor->y - editor->py) == (editor->rows - 1))
            editor->py++;
        editor->px = 0;
    }
    editor->x = 0;
}

Editor* edNew() {
    return edNewStr("untitled.txt");
}

Editor* edNewStr(const char* filename) {
    Editor *editor = (Editor*)malloc(sizeof(Editor));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &(editor->win));
    editor->x = editor->y = editor->px = editor->py = 0;
    editor->cols = editor->win.ws_col;
    editor->rows = editor->win.ws_row;
    editor->filename = filename;
    editor->status = "--NORMAL--";
    editor->mode = 'n';
    editor->buff = bufNew();
    editor->pad = newpad(EDITOR_MAX_ROWS, EDITOR_MAX_ROWS);
    editor->padstatus = newpad(1, COLS);
    keypad(editor->pad, true);

    FILE *fp = fopen(filename, "r");

    if (fp != NULL) {
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, fp) != -1) {
            bufAppendLine(editor->buff, line);
            line = NULL;
        }
        fclose(fp);
        if (editor->buff->size == 0) {
            bufAppendLine(editor->buff, strdup(""));
        }
    }
    else {
        bufAppendLine(editor->buff, strdup(""));
    }

    return editor;
}

void edSaveFile(Editor *editor) {
    FILE *fp = fopen(editor->filename, "w");

    if (fp != NULL) {
        for (int i = 0; i < editor->buff->size; i++)
        {
            char *s = editor->buff->lines[i];
            fwrite(s, sizeof(s[0]), strlen(s), fp);
        }
        editor->status = "Saved to file!";
    }
    else {
        editor->status = "Error: Cannot open file for writing!";
    }
    fclose(fp);
}

char* ctos(Editor *editor, int n) {
    char *str;
    int len = snprintf(NULL, 0, "%d", n);
    str = (char*)malloc(len + 1);
    snprintf(str, len + 1, "%d", n);

    return str;
}

void edUpdateWindow(Editor *editor) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int y, x;
    y = w.ws_row;
    x = w.ws_col;
    if ((editor->rows != y) || (editor->cols != x)) {
        editor->rows = y;
        editor->cols = x;
        erase();
    }
    refresh();
}

void edHandleNormalMode(Editor *editor, int c) {
    switch (c) {
        case 'x':
            editor->mode = 'x';
            break;
        case 'i':
            editor->mode = 'i';
            break;
        case 's':
            //save file
            edSaveFile(editor);
            break;
    }
}

void edHandleInsertMode(Editor *editor, int c) {
    switch (c) {
        case 27:
            editor->mode = 'n';
            break;
        case 127:
        case KEY_BACKSPACE:
            // The Backspace key
            edDelete(editor);
            break;
        case KEY_DC:
            // The Delete key
            break;
        case KEY_ENTER:
        case 10:
            // The Enter key
            edEnterReset(editor);
            break;
        case KEY_BTAB:
        case KEY_CTAB:
        case KEY_STAB:
        case KEY_CATAB:
        case 9:
            // The Tab key
            strInsert(&(editor->buff->lines[editor->y]), editor->x + 1, (4 - (editor->x % 4)), ' ');
            editor->x += (4 - (editor->x % 4));
            break;
        default:
            // Any other character
            if (editor->y != (EDITOR_MAX_ROWS - 1) || editor->x != (EDITOR_MAX_COLS - 1)) {
                strInsert(&(editor->buff->lines[editor->y]), editor->x + 1, 1, c);
                
                int linesize = strlen(editor->buff->lines[editor->y]);
                if (linesize == (EDITOR_MAX_COLS) && (editor->y < (EDITOR_MAX_ROWS - 1))) {
                    editor->x++;
                    edEnterReset(editor);
                }
                else {
                    editor->x++;
                    if ((editor->x - editor->px) == (editor->cols)) {
                        editor->px++;
                    }
                }
            }
            break;
    }
}

void edHandleInput(Editor* editor, int c) {
    switch (c) {
    case KEY_UP:
        edMoveUp(editor);
        break;
    case KEY_DOWN:
        edMoveDown(editor);
        break;
    case KEY_LEFT:
        edMoveLeft(editor);
        break;
    case KEY_RIGHT:
        edMoveRight(editor);
        break;
    default:
        switch (editor->mode)
        {
        case 'n':
            edHandleNormalMode(editor, c);
            break;
        case 'i':
            edHandleInsertMode(editor, c);
            break;
        default:
            break;
        }
        break;
    }
    
}

void edPrintBuff(Editor* editor) {
    int i = editor->py;
    while (i < (editor->py + editor->rows)) {
        if (i >= editor->buff->size) {
            mvwprintw(editor->pad, i, 0, "%s", "~");
        }
        else {
            mvwprintw(editor->pad, i, 0, "%s", editor->buff->lines[i]);
        }
        wclrtoeol(editor->pad);
        i++;
    }
    wmove(editor->pad, editor->y, editor->x);
    prefresh(editor->pad, editor->py, editor->px, 0, 0, editor->rows - 2, editor->cols - 1);
}

void edPrintStatusLine(Editor* editor) {
    wattron(editor->padstatus, A_REVERSE);
    
    mvwprintw(editor->padstatus, 0, 0, editor->status);
    wclrtoeol(editor->padstatus);
    
    int len = snprintf(NULL, 0, "COL: %s  ROW: %s",  ctos(editor, editor->x), ctos(editor, editor->y));
    mvwprintw(editor->padstatus, 0, editor->cols - len , "COL: %s  ROW: %s",  ctos(editor, editor->x), ctos(editor, editor->y));
    wclrtoeol(editor->padstatus);
    
    wattroff(editor->padstatus, A_REVERSE);
    prefresh(editor->padstatus, 0, 0, editor->rows - 1, 0, editor->rows - 1, editor->cols);
}

void edUpdateStatus(Editor* editor) {
    switch (editor->mode)
    {
    case 'n':
        editor->status = "--NORMAL--";
        break;
    case 'i':
        editor->status = "--INSERT--";
        break;
    case 'x':
        editor->status = "--EXITING--";
        break;
    default:
        break;
    }
}

void edMoveRight(Editor *editor) {
    if (((editor->x == (EDITOR_MAX_COLS - 1)) || 
        (editor->x == strlen(editor->buff->lines[editor->y]))) && (editor->y < (editor->buff->size - 1))) {
        editor->x = 0;
        editor->px = 0;
        if (editor->y < (EDITOR_MAX_ROWS - 1)) {
            editor->y++;
            if ((editor->y - editor->py) == (editor->rows - 1)) {
                editor->py++;
            }
        }
    }
    else if ((editor->x - editor->px) == (editor->cols - 1)) {
           editor->x++;
           editor->px++; 
    }
    else {
        if (editor->x < strlen(editor->buff->lines[editor->y])) 
            editor->x++;
    }
}

void edMoveLeft(Editor *editor) {
    if (editor->x == 0) {
        if (editor->y > 0) {
            editor->y--;
            editor->x = strlen(editor->buff->lines[editor->y] - 1);
            if ((editor->y - editor->py) == 0) {
                editor->py--;
            }
            if (editor->x > (editor->cols - 1)) {
                editor->px = (editor->x - editor->cols);
            }
            else {
                editor->px = 0;
            }
        }
    }
    else if ((editor->x - editor->px) == (editor->cols - 1)) {
        editor->x--;
        editor->px--;
    }
    else {
        editor->x--;
    }
}

void edMoveUp(Editor *editor) {
    if (editor->y > 0) {
        editor->y--;
        if ((editor->y - editor->py) == 0) {
            editor->py--;
        }
        if (editor->x > strlen(editor->buff->lines[editor->y])) {
            editor->x = strlen(editor->buff->lines[editor->y]);
            if ((editor->x - editor->px) < 0) {
                editor->px = editor->x - 1;
            }
        }
    }
}

void edMoveDown(Editor *editor) {
    if (editor->y < editor->buff->size - 1) {
        editor->y++;
        if ((editor->y - editor->py) == editor->rows - 1) {
            editor->py++;
        }
        if (editor->x > strlen(editor->buff->lines[editor->y])) {
            editor->x = strlen(editor->buff->lines[editor->y]);
            if ((editor->x - editor->px) < 0) {
                editor->px = editor->x - 1;
            }
        }
    }
}