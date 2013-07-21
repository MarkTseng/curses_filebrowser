#include <dirent.h>
#include <pwd.h>
#include <curses.h>
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

int file_browser_is_root(char *sz)
{
	return (strcmp(sz, "/") == 0);
}

void append_path(char *sz_dest, char *file)
{
	while (file && *file == '/') {
		file++;
	}

	if (sz_dest[strlen(sz_dest) - 1] == '/')
		sz_dest[strlen(sz_dest) - 1] = '\0';

	strcat(sz_dest, "/");
	strcat(sz_dest, file);
}

MenuModule *file_browser_init_window(WINDOW * window)
{
	int x;
	FileBrowserModule *menu = malloc(sizeof(FileBrowserModule));
	memset(menu, 0, sizeof(FileBrowserModule));

	struct passwd *pw = getpwuid(getuid());
	strcpy(menu->path, pw->pw_dir);

	menu->folder_focus = 1;
	menu->module.window = window;

	/*
	 * Keypress
	 */
	menu->module.keypress = file_browser_keypress;
	menu->module.draw = file_browser_draw;

	/* 
	 * calculate locations
	 */
	file_browser_recalc_screen_size(menu);

	menu->selected_folder = 0;
	menu->selected_file = 0;
	menu->fileoffset = 0;
	menu->folderoffset = 0;
	menu->foldercount = 0;
	menu->dirs = 0;
	menu->files = 0;
	menu->spare = 0;

	file_browser_build_filedir_lists(menu);

	/* 
	 * draw the module
	 */
	file_browser_draw_worker(menu);

	refresh();

	return (MenuModule *) menu;
}

void add_node(char *sz, direntry ** list, direntry ** spare)
{
	direntry *entry;
	/*
	 * allocate.
	 */
	entry = malloc(sizeof(direntry));

	/*
	 * zero the entry.
	 */
	memset(entry, 0, sizeof(direntry));
	/*
	 * copy the string to the entry.
	 */
	strcpy(entry->d_name, sz);
	/*
	 * list is empty make it the head.
	 */
	if (!(*list)) {
		*list = entry;
		return;
	}
	/*
	 * loop through the list until the last node
	 */
	direntry *node = *list;
	while (node->next) {

		node = node->next;

	}
	/*
	 * 
	 */
	node->next = entry;
	entry->prev = node;

}

void clear_list(direntry ** list, direntry ** spares)
{
	if (!(*list))
		return;

	direntry *node = *list;
	while (node) {
		direntry *x = node;
		node = x->next;
		free(x);
	}

	*list = 0;

}

void file_browser_build_filedir_lists(FileBrowserModule * module)
{
	clear_list(&module->dirs, &module->spare);
	clear_list(&module->files, &module->spare);
	int n, c;
	struct dirent **namelist;

	/*
	 * Add a .. directory so the user can step up a folder
	 */
	if (!file_browser_is_root(module->path)) {
		add_node("..", &module->dirs, &module->spare);
		module->foldercount++;
	}

	/*
	 * Scan each folder in the current folder and add to 
	 * linked list
	 */
	n = 0;
	module->foldercount = 0;
	c = scandir(module->path, &namelist, 0, alphasort);
	while (n < c) {
		if (namelist[n] && namelist[n]->d_type == DT_DIR
		    && namelist[n]->d_name && namelist[n]->d_name[0] != '.') {
			add_node(namelist[n]->d_name, &module->dirs,
				 &module->spare);
			module->foldercount++;
		}
		free(namelist[n]);
		n++;
	}
	free(namelist);

	/*
	 * Scan each folder in the current folder and add to 
	 * linked list
	 */
	n = 0;
	module->filecount = 0;
	c = scandir(module->path, &namelist, 0, alphasort);
	while (n < c) {
		if (namelist[n] && namelist[n]->d_type == DT_REG
		    && namelist[n]->d_name && namelist[n]->d_name[0] != '.') {
			add_node(namelist[n]->d_name, &module->files,
				 &module->spare);
			module->filecount++;
		}
		free(namelist[n]);
		n++;
	}
	free(namelist);
}

void file_browser_recalc_screen_size(FileBrowserModule * module)
{
	/* 
	 * Get terminal size 
	 */
	getmaxyx(module->module.window, module->module.szy, module->module.szx);

	/* 
	 * Calculate the center of the window 
	 */
	module->center = module->module.szx / 2;

	module->maxcount = module->module.szy - 2;
}

/*
 * Handles Key presses.
 */
void file_browser_keypress(MenuModule * module, int key)
{
	FileBrowserModule *menu = (FileBrowserModule *) module;
	int folderchanged = 0;
	DIR *d;
	struct dirent *dir;

	switch (key) {
		/*
		 * On TAB press. switch focus between folder or file pane.
		 */
	case 9:
		menu->folder_focus = !menu->folder_focus;
		module->dirty = 1;
		break;
		/*
		 * On key press down. Either move down the folder or 
		 * file list depending on which one is focused.
		 */
	case KEY_DOWN:
		if (menu->folder_focus) {
			int mod = file_browser_is_root(menu->path) ? 0 : 1;
			menu->selected_folder++;
			if (menu->selected_folder >= menu->foldercount) {	//888.com
				menu->selected_folder = 0;
				menu->folderoffset = 0;
			} else if (menu->selected_folder >=
				   menu->maxcount + menu->folderoffset) {
				menu->folderoffset++;
			}
		} else {
			menu->selected_file++;
			if (menu->selected_file >= menu->filecount) {
				menu->selected_file = 0;
				menu->fileoffset = 0;
			} else if (menu->selected_file >=
				   menu->maxcount - 1 + menu->fileoffset) {
				menu->fileoffset++;
			}
		}
		module->dirty = 1;
		break;
		/*
		 * On key press up. Either move up the folder or 
		 * file list depending on which one is focused.
		 */
	case KEY_UP:
		if (menu->folder_focus) {
			menu->selected_folder--;
			int mod = file_browser_is_root(menu->path) ? 0 : 1;
			if (menu->selected_folder < 0) {
				menu->selected_folder = menu->foldercount - 1;
				if (menu->foldercount > menu->maxcount) {
					menu->folderoffset =
					    (menu->foldercount -
					     menu->maxcount);

				}	//     menu->folderoffset = 0; 
				//   menu->foldercount - menu->maxcount;

			} else if (menu->folderoffset > 0) {
				menu->folderoffset--;
			}
		} else {
			menu->selected_file--;
			if (menu->selected_file < 0) {
				menu->selected_file = menu->filecount - 1;
				if (menu->filecount > menu->maxcount)
					menu->fileoffset =
					    menu->filecount - menu->maxcount;
			} else if (menu->fileoffset > 0) {
				menu->fileoffset--;
			}
		}
		module->dirty = 1;
		break;
	case 'u':
		{
			char *dir_name = dirname(menu->path);
			strcpy(menu->path, dir_name);
			file_browser_build_filedir_lists(menu);
			module->dirty = 1;
		}
		break;
	case 10:
	case KEY_ENTER:
	case 13:

		if (menu->folder_focus) {
			direntry *d = menu->dirs;
			int index = 0;
			while (d) {
				if (index == menu->selected_folder) {
					if (strcmp(d->d_name, "..") == 0) {

						char *dir_name =
						    dirname(menu->path);
						strcpy(menu->path, dir_name);

					} else {
						append_path(menu->path,
							    d->d_name);
						menu->selected_folder = 0;
					}
					break;
				}
				index++;
				d = d->next;
			}

			file_browser_build_filedir_lists(menu);
			module->dirty = true;
		}

		break;

	}

}

void file_browser_draw(MenuModule * module)
{
	/*
	 * Has anything changed?
	 * If so redraw.
	 */
	FileBrowserModule *editor = (FileBrowserModule *) module;
	if (module->dirty) {
		module->dirty = 0;
		menu_clear(editor, 1);
		file_browser_draw_worker(editor);
	}
}

void file_browser_draw_files(FileBrowserModule * module)
{
	int y = 0;
	int index = 0;
	direntry *f = module->files;
	if (f) {
		while (f) {
			if (y < module->maxcount && index >= module->fileoffset) {

				menu_draw_text(module,
					       y + 1,
					       module->center + 1,
					       module->center - 1,
					       f->d_name,
					       (index ==
						module->selected_file
						&& !module->folder_focus)
					       ? 3 : 2);

				y++;

			}
			index++;
			f = f->next;
		}
	}
	while (y < module->maxcount) {
		menu_draw_text(module, y + 1, module->center + 1,
			       module->center - 1, "", 2);
		y++;
	}
}

void file_browser_draw_folders(FileBrowserModule * module)
{
	int y = 0;
	int index = 0;
	direntry *d = module->dirs;
	if (d) {
		while (d) {
			if (y < module->maxcount
			    && index >= module->folderoffset) {

				menu_draw_text(module,
					       y + 1,
					       1,
					       module->center - 2,
					       d->d_name,
					       (index ==
						module->selected_folder
						&& module->folder_focus)
					       ? 3 : 2);

				y++;
			}
			index++;
			d = d->next;
		}
	}
	while (y < module->maxcount) {
		menu_draw_text(module, y + 1, 1, module->center - 2, "", 2);
		y++;
	}

}

void file_browser_draw_worker(FileBrowserModule * module)
{
	/*
	 * Draw the surrounding boxes around file and folder sections.
	 */
	menu_draw_box(0, 0, module->module.szy - 1, module->center - 1, 0);
	menu_draw_box(0,
		      module->center, module->module.szy - 1, module->center,
		      0);

	/*
	 * Draw all the folders in the current folder.
	 */
	file_browser_draw_folders(module);

	/*
	 * Draw all the files in the current folder.
	 */
	file_browser_draw_files(module);

}
