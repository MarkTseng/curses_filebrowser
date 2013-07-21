CC=gcc
LD=gcc
CFLAGS=-c -Wall
LDFLAGS=-L/usr/lib -lncurses -lpanel
SOURCES=file_browser.c menu_common.c main.c
HEADERS=menu.h file_browser.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=browse

all: $(SOURCES) $(EXECUTABLE) $(HEADERS)
	
$(EXECUTABLE): $(OBJECTS) 
	$(LD) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm $(OBJECTS) -f

