#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <termios.h>
#include <unistd.h>
#include "menu.h"
#include "file_browser.h"

/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

MenuModule *file_browser_init_window(WINDOW * window);


int main(int argc, char **argv)
{
	/* 
	 * initialize ncurses
	 */
	WINDOW *window = initscr();
	curs_set(0);
	cbreak();
	noecho();
	nodelay(window, 10);
	leaveok(window, TRUE);
	keypad(window, TRUE);
	start_color();

	/* 
	 * init the colors for ncurses
	 */
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	init_pair(4, COLOR_WHITE, COLOR_WHITE);

	MenuModule *mod = (MenuModule *) file_browser_init_window(window);	//wave_editor_init_window(window);

	int c = 0;
	
	while (1) {
		c = wgetch(window);

		if (c == ERR)
			continue;

		if (c == 'q' || c == 'Q') {
			break;
		}
		while (c != ERR) {
			mod->keypress(mod, c);
			c = wgetch(window);
		}
		mod->draw(mod);
	}
	endwin();
	return 0;
}
