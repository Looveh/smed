#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define MAXROWS 1000
#define TEXT_WIN_LEFT 5
#define LNR_WIN_WIDTH 4

WINDOW * statwin;
WINDOW * textwin;
WINDOW * lnrwin;

char ** buf;

int nbrows;
int cx, cy; // cursor coordinates

void insert_char(char ch, int row, int col){
	int plen, i;
	char * nrow;
	plen = strlen(buf[row]) + 1;
	nrow = (char *) malloc(sizeof(char)*(plen+1));

	i = 0;
	while(i < col){
		nrow[i] = buf[row][i];
		i++;
	}
	nrow[i] = ch;
	while(i < plen){
		nrow[i+1] = buf[row][i];
		i++;
	}

	free(buf[row]);
	buf[row] = nrow;
}

void delete_char(int row, int col){
	int plen, i;
	char * nrow;
	plen = strlen(buf[row]) + 1;
	
	// cant delete char outside of line
	if(col < 0 || col >= plen-1)
		return;
	
	nrow = (char *) malloc(sizeof(char)*(plen-1));

	for(i = 0; i < plen; i++){
		if(i < col)
			nrow[i] = buf[row][i];
		else if(i > col)
			nrow[i-1] = buf[row][i];
	}

	free(buf[row]);
	buf[row] = nrow;
}

void init_buf(){
	char msg1[] = "This is a row";
	char msg2[] = "And this is a second row";
	char msg3[] = "... and this is row three";
	buf[0] = (char *) malloc(sizeof(msg1));
	buf[1] = (char *) malloc(sizeof(msg2));
	buf[2] = (char *) malloc(sizeof(msg3));
	strcpy(buf[0], msg1);
	strcpy(buf[1], msg2);
	strcpy(buf[2], msg3);
	nbrows = 3;
}

void pbuf(){
	int i;
	for(i = 0; i < nbrows; i++){
		mvwprintw(textwin, i, 0, "                                                                 ");
		mvwprintw(textwin, i, 0, "%s", buf[i]);
	}
	for(i = 0; i < LINES; i++){
		mvwprintw(lnrwin, i, 0, "   |");
		if(i < nbrows) mvwprintw(lnrwin, i, 0, "%d", i+1);
		else 					 mvwprintw(lnrwin, i, 0, "~");
	}
	wrefresh(textwin);
	wrefresh(lnrwin);
}

int main(int argc, char ** argv){
	int i;
	int ch;
	buf = (char**) malloc(MAXROWS*sizeof(char*));
	init_buf();

	cx = 0; cy = TEXT_WIN_LEFT;	
	initscr();
	raw();
	noecho();
	
	refresh();
	
	//statwin = newwin(1, COLS, LINES-1, 0);
	textwin = newwin(LINES, COLS, 0, TEXT_WIN_LEFT);
	lnrwin = newwin(LINES, LNR_WIN_WIDTH, 0, 0);
	keypad(textwin, TRUE);
	wrefresh(textwin);
	wrefresh(lnrwin);
	

	while((ch = wgetch(textwin)) != 'q'){
		switch(ch){
			case KEY_UP:
				if(cx > 0) cx--;
				break;
			case KEY_DOWN:
				if(cx < nbrows-1) cx++;
				break;
			case KEY_LEFT:
				if(cy > 0) cy--;
				break;
			case KEY_RIGHT:
				cy++;
				break;
			case KEY_DC:
				delete_char(cx, cy);
				break;
			case 127: // backspace, KEY_BACKSPACE doesnt work
				delete_char(cx, cy-1);
				cy--;
				break;
			default:
				insert_char(ch, cx, cy);
				cy++;
				break;
		}
		if(cy < 0) cy = 0;
		if(cy > strlen(buf[cx])) cy = strlen(buf[cx]);
		move(cx, cy + TEXT_WIN_LEFT);
		pbuf();
		refresh();
	}
	
	endwin();
	return(0);
}

