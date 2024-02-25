static const char *font = "Liberation Sans:size=12";

#define BACKGROUND 0xffffff

/* Foreground. */
#define FG_RED 0x0000
#define FG_GREEN 0x0000
#define FG_BLUE 0x0000

#define X 0
#define Y 0
#define WIDTH 300
#define HEIGHT 45

#define PADDING 5

static const char *volp[] = { "pamixer", "-i", "5", NULL };
static const char *volm[] = { "pamixer", "-d", "5", NULL };
static const char *mute[] = { "pamixer", "-t", NULL };
static const char *brip[] = { "brightnessctl", "set", "+5%", NULL };
static const char *brim[] = { "brightnessctl", "set", "5%-", NULL };

static Command commands[] = {
	{ "vol+", volp, 4 },
	{ "vol-", volm, 4 },
	{ "mute", mute, 4 },
	{ "bri+", brip, 4 },
	{ "bri-", brim, 4 },
};
