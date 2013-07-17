/* radare - LGPL - Copyright 2013 - pancake */
/* ansi 256 color extension for r_cons */

#include <r_cons.h>

// TODO: move to r_num_round
static inline int cast(double d) {
	return (int)d + ((((int)((d - (int)d)*10))>5)? 1: 0);
}

static int gs (int rgb) {
	return 232 + (double)rgb/(255/24.1);
}

static int rgb(int r, int g, int b) {
	const double k = (256/6);
	r = R_DIM (r/k, 0, 6);
	g = R_DIM (g/k, 0, 6);
	b = R_DIM (b/k, 0, 6);
	return 16 + (r*36) + (g*6) + b;
}

static inline void rgbinit(int r, int g, int b) {
#if __UNIX__
	r_cons_printf ("\x1b]4;%d;rgb:%2.2x/%2.2x/%2.2x\x1b\\",
			16 + (r * 36) + (g * 6) + b,
			(r ? (r * 40 + 55) : 0),
			(g ? (g * 40 + 55) : 0),
			(b ? (b * 40 + 55) : 0));
#endif
}

R_API void r_cons_rgb_init (void) {
	int r, g, b;
	for (r = 0; r < 6; r++)
		for (g = 0; g < 6; g++)
			for (b = 0; b < 6; b++)
				rgbinit (r, g, b);
}

R_API int r_cons_rgb_parse (const char *p, ut8 *r, ut8 *g, ut8 *b, int *is_bg) {
	//const double k = (256/6);
	if (!p) return 0;
	if (*p==0x1b) p++;
	if (*p!='[') return 0;
	if (p[3]==';') {
		int R,G,B,n = atoi (p+6);
		n-=16;
		B = (n&3) * 2.3;
		G = ((n>>3)&3) * 2.3;
		R = ((n>>6)&3) * 2.3;
		
		if (r) *r = R* 42.2;
		if (g) *g = G*42.2;
		if (b) *b = B*42.2;
		//n = (r*36) + (g*6) + b;
		// b = n - (r*36) - (g*6)
		// r = (n-b-(g*6))/36
		if (is_bg) *is_bg = (atoi (p+1) == 48)?1:0;
		return 1;
	}
	// TODO support ansi
	return 0;
}

R_API char *r_cons_rgb_str (char *outstr, ut8 r, ut8 g, ut8 b, int is_bg) {
	int k, fgbg = is_bg? 48: 38;
	k = (r == g && g == b)?  gs (r): rgb (r, g, b);
	//k = rgb (r, g, b);
	if (!outstr) outstr = malloc (32);
	sprintf (outstr, "\x1b[%d;5;%dm", fgbg, k);
	return outstr;
}

R_API void r_cons_rgb (ut8 r, ut8 g, ut8 b, int is_bg) {
#if __WINDOWS__
	#warning r_cons_rgb not yet supported on windows
#else
	char outstr[64];
	r_cons_strcat (r_cons_rgb_str (outstr, r, g, b, is_bg));
#endif
}

R_API void r_cons_rgb_fgbg (ut8 r, ut8 g, ut8 b, ut8 R, ut8 G, ut8 B) {
	r_cons_rgb (r, g, b, 0);
	r_cons_rgb (R, G, B, 1);
}
