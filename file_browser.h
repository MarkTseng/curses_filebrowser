#ifndef __FILE_BROWSER__H__
#define __FILE_BROWSER__H__

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

typedef struct direntry direntry;
typedef struct direntry {
	char d_name[256];
	direntry *next, *prev;
} direntry;

typedef struct {
	/*
	 * File browser windowing
	 */
	MenuModule module;
	Rect editorRect;

	int center;

	char path[FILENAME_MAX];

	int selected_folder, selected_file;

	int folder_focus;	/* 1 = folder 0 = file */

	int foldercount;
	int filecount;

	int fileoffset;
	int folderoffset;

	int maxcount;

	direntry *dirs;
	direntry *files;
	direntry *spare;
} FileBrowserModule;

void file_browser_recalc_screen_size(FileBrowserModule * module);
void file_browser_keypress(MenuModule * module, int key);
void file_browser_draw(MenuModule * module);

#endif				//__FILE_BROWSER__H__
