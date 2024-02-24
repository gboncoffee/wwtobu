#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define LENGTH(X) (sizeof X / sizeof X[0])

typedef struct {
	char *label;
	char **command;
	int size;
} Command;

#include "config.h"

void handler(int _d)
{
	wait(NULL);
}

int main()
{
	Display *display;
	XEvent event;
	XftDraw	*xftdraw;
	XRenderColor xrcolor;
	XftColor xftcolor;
	XftFont	*xftfont = NULL;
	XGlyphInfo extents;
	int cmds_x[LENGTH(commands)] = { 0 };
	int cmds_w[LENGTH(commands)] = { 0 };
	int y;
	int h;
	XButtonEvent e;

	signal(SIGCHLD, handler);

	display = XOpenDisplay(0);
	Window w = XCreateSimpleWindow(display, DefaultRootWindow(display), X, Y, WIDTH, HEIGHT, 0, 0, BACKGROUND);
	XStoreName(display, w, "Window Waiting TO Be Usefull");
	XSelectInput(display, w, ExposureMask | ButtonPressMask);
	XMapWindow(display, w);

	xftfont = XftFontOpenName(display, 0, font);
	if (!xftfont)
		return 1;


	xrcolor.red = FG_RED;
	xrcolor.green = FG_GREEN;
	xrcolor.blue = FG_BLUE;
	xrcolor.alpha = 0xffff;
	XftColorAllocValue(display, DefaultVisual(display, 0), DefaultColormap(display, 0), &xrcolor, &xftcolor);

	XftTextExtentsUtf8(display, xftfont, commands[0].label, commands[0].size, &extents);
	cmds_x[0] = PADDING + extents.x;
	cmds_w[0] = extents.width - extents.x;
	y = PADDING + extents.y;
	h = extents.height;
	for (int i = 1; i < LENGTH(commands); i++) {
		XftTextExtentsUtf8(display, xftfont, commands[i].label, commands[i].size, &extents);
		cmds_x[i] = PADDING + extents.x + cmds_x[i - 1] + cmds_w[i - 1];
		cmds_w[i] = extents.width - extents.x;
	}

	for (;;) {
		XNextEvent(display, &event);
		switch (event.type) {
		case Expose:
			XClearWindow(display, w);
			xftdraw = XftDrawCreate(display, w, DefaultVisual(display, 0), DefaultColormap(display, 0));

			for (int i = 0; i < LENGTH(commands); i++)
				XftDrawStringUtf8(xftdraw, &xftcolor, xftfont, cmds_x[i], y, commands[i].label, commands[i].size);
			XftDrawDestroy(xftdraw);
			break;
		case ButtonPress:
			e = event.xbutton;
			if (e.y > PADDING && e.y < PADDING + h) {
				for (int i = 0; i < LENGTH(commands); i++) {
					if (e.x >= cmds_x[i] && e.x <= cmds_w[i] + cmds_x[i]) {
						if (fork() == 0) {
							execvp(commands[i].command[0], commands[i].command);
							exit(1);
						}
						break;
					}
				}
			}
			break;
		}
	}


	XFlush(display);

	XftColorFree(display, DefaultVisual(display,0), DefaultColormap(display,0), &xftcolor);

	return 0;
}
