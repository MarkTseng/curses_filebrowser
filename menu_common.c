#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <curses.h>
#include <panel.h>
#include <signal.h>
#include "menu.h"

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

void
menu_draw_text(MenuModule * module, int y, int x, int w, char *text, int color)
{
	int c = 0;

	attrset(COLOR_PAIR(color));
	while (c < w) {
		mvaddch(y, x++, *text != 0 ? *text : ' ');
		if (*text != 0)
			text++;
		c++;
	}
}

void menu_clear(MenuModule * module, int full_redraw)
{
	int x, y;
	int f = full_redraw ? 0 : 1;

	if (full_redraw)
		clearok(module->window, TRUE);

	/* buggy ncurses doesn't really write spaces with the specified
	 * color into the screen on clear () or erase ()
	 */
	for (x = f; x < module->szx - f; x++)
		for (y = f; y < module->szy - f; y++)
			mvaddch(y, x, ' ');
}

void menu_draw_box(int y, int x, int h, int w, int color)
{

	fprintf(stderr, "DRAW_BOX:x=%d\n", x);
	fprintf(stderr, "DRAW_BOX:y=%d\n", y);
	fprintf(stderr, "DRAW_BOX:h=%d\n", h);
	fprintf(stderr, "DRAW_BOX:w=%d\n", w);
	int i;
	attrset(COLOR_PAIR(color));
	/* Verticle lines
	 */
	for (i = y; i < y + h; i++) {
		mvaddch(i, x, ACS_VLINE);
		mvaddch(i, x + w, ACS_VLINE);
	}
	/* Horizontal lines
	 */
	for (i = x; i < x + w; i++) {
		mvaddch(y, i, ACS_HLINE);
		mvaddch(y + h, i, ACS_HLINE);
	}

	/* corners
	 */
	mvaddch(y, x, ACS_ULCORNER);
	mvaddch(y, x + w, ACS_URCORNER);
	mvaddch(y + h, x, ACS_LLCORNER);
	mvaddch(y + h, x + w, ACS_LRCORNER);
}

void menu_draw_line_v(int x, int y, int h, int color)
{
	fprintf(stderr, "VLINE x=%d,y=%d,h=%d,color=%d\n", x, y, h, color);
	int i;
	fprintf(stderr, "ATTR\n");
	attrset(COLOR_PAIR(color));
	fprintf(stderr, "PATTR\n");
	for (i = 0; i < h; i++) {
		mvaddch(y + i, x, ACS_VLINE);
	}
	fprintf(stderr, "PFOR\n");

}

void menu_draw_line_h(int x, int y, int w, int color)
{
	int i;
	attrset(COLOR_PAIR(color));
	for (i = 0; i < w; i++) {
		mvaddch(y, x + i, ACS_HLINE);
	}
}

int sign(int x)
{
	if (x < 0)
		return -1;
	return 1;
}

void plot(int x, int y, int ch)
{
	mvaddch(y, x, ch);
}

int posx, posy;
void menu_draw_moveto(int x, int y)
{
	posx = x;
	posy = y;
}

void menu_draw_line(int from_x, int from_y, int x2, int y2, char ch, int color)
{
	int dx, dy;
	int ax, ay;
	int sx, sy;
	int x, y;
	int d;

	attrset(COLOR_PAIR(color));

	fprintf(stderr, "from_x=%d,from_y=%d,x2=%d,y2=%d,ch=%d,color=%d\n",
		from_x, from_y, x2, y2, ch, color);

	dx = x2 - posx;
	dy = y2 - from_y;

	ax = abs(dx * 2);
	ay = abs(dy * 2);

	sx = sign(dx);
	sy = sign(dy);

	x = from_x;
	y = from_y;

	if (ax > ay) {
		d = ay - (ax / 2);

		while (1) {
			fprintf(stderr, "linex->x=%d\n", x);
			fprintf(stderr, "linex->y=%d\n", y);
			fprintf(stderr, "linex->x2=%d\n", x2);
			plot(x, y, ch);
			if (x == x2)
				return;

			if (d >= 0) {
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	} else {
		d = ax - (ay / 2);

		while (1) {
			fprintf(stderr, "liney->x=%d\n", x);
			fprintf(stderr, "liney->y=%d\n", y);
			fprintf(stderr, "liney->y2=%d\n", y2);
			plot(x, y, ch);
			if (y == y2)
				return;

			if (d >= 0) {
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
}
