#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "editor.h"

void cursesInit() {
    initscr();
	noecho();
	cbreak();
	set_escdelay(0);
	keypad(stdscr, true);
}

int main(int argc, char const *argv[])
{
    cursesInit();
    Editor *editor;
    if (argc > 1) {
        editor = edNewStr(argv[1]);
    }
    else {
        editor = edNew();
    }
    

    while (editor->mode != 'x') {
        edUpdateWindow(editor);
        edUpdateStatus(editor);
        edPrintStatusLine(editor);
        
        edPrintBuff(editor);
        edHandleInput(editor, wgetch(editor->pad));
        edPrintBuff(editor);
    }  
    edDestroy(editor); 
    endwin();
    exit(EXIT_SUCCESS);
}
