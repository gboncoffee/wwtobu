CC = tcc
CFLAGS = -Wall -g
CINCS = -I/usr/include/freetype2
CLIBS = -lX11 -lXft

wwtobu: main.c
	$(CC) $(CINCS) $(CFLAGS) $(CLIBS) -o $@ $^
