#ifndef __MODULE__MENU__H__
#define __MODULE__MENU__H__
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
typedef struct MenuModule MenuModule;

typedef struct {
	int x, y, w, h;
} Rect;

typedef struct MenuModule {
	void (*keypress) (MenuModule * module, int key);
	void (*draw) (MenuModule * module);
	int szx, szy;

	int dirty;
	WINDOW *window;

} MenuModule;

MenuModule *editor_init_window(void);

/* Common Functions */

void menu_draw_box(int y, int x, int h, int w, int color);
void menu_draw_line_v(int x, int y, int h, int color);
void menu_draw_line_h(int x, int y, int w, int color);
void menu_draw_line(int from_x, int from_y, int x2, int y2, char ch, int color);
void menu_draw_moveto(int x, int y);
void menu_draw_text(MenuModule * module, int y, int x, int w, char *text,
		    int color);
#endif				// #ifdef __MODULE__MENU__H__
