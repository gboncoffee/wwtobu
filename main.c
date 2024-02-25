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

Display *display;
Window w;
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
char *status = NULL;

void handler(int _d)
{
	wait(NULL);
}

void update_status(void)
{
	XTextProperty prop;
	if (status != NULL)
		XFree(status);
	status = NULL;
	if (XGetWMName(display, DefaultRootWindow(display), &prop))
		status = (char*) prop.value;
}

void render(void)
{
	XClearWindow(display, w);
	xftdraw = XftDrawCreate(display, w, DefaultVisual(display, 0), DefaultColormap(display, 0));

	if (status != NULL)
		XftDrawStringUtf8(xftdraw, &xftcolor, xftfont, PADDING, y, status, strlen(status));

	for (int i = 0; i < LENGTH(commands); i++)
		XftDrawStringUtf8(xftdraw, &xftcolor, xftfont, cmds_x[i], y + PADDING * 2 + h, commands[i].label, commands[i].size);
	XftDrawDestroy(xftdraw);
}

int main()
{
	signal(SIGCHLD, handler);

	display = XOpenDisplay(0);
	w = XCreateSimpleWindow(display, DefaultRootWindow(display), X, Y, WIDTH, HEIGHT, 0, 0, BACKGROUND);
	XStoreName(display, w, "Window Waiting TO Be Usefull");
	XSelectInput(display, w, ExposureMask | ButtonPressMask);
	XSelectInput(display, DefaultRootWindow(display), PropertyChangeMask);
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

	update_status();

	for (;;) {
		XNextEvent(display, &event);
		switch (event.type) {
		case PropertyNotify:
			update_status();
		case Expose:
			render();
			break;
		case ButtonPress:
			e = event.xbutton;
			if (e.y > PADDING * 3 + h && e.y < PADDING * 4 + h * 2) {
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
