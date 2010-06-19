#include "rgb.h"
#include "float.h"

// Color functions.
#define CLAMP(x) (x<0?0:x>255?255:x)
int rgb(int r, int g, int b) {
	return CLAMP(r)|CLAMP(g)<<8|CLAMP(b)<<16;
}
int rgb(float r, float g, float b) {
	return rgb(fist(r),fist(g),fist(b));
}
int rgbu(float r, float g, float b) {
	return rgb(r*255,g*255,b*255);
}

int rgbl(float t, int a, int b) {
	float s=1-t;
	unsigned char * c = (unsigned char*)&a;
	unsigned char * d = (unsigned char*)&b;
	return rgb(
		c[0]*s + d[0]*t, 
		c[1]*s + d[1]*t, 
		c[2]*s + d[2]*t
	);
}

int grad(Gradient * g, int length, float pos) {
	int a=0;
	int b=length;
	if (pos<g[0].pos) return g[0].color;
	while (b-a>1) {
		int c = (a+b)/2;
		if (g[c].pos<pos) a = c;
		else b = c;
	}
	// Here a == b-1
	if (b==length) return g[a].color;
	float t=(pos-g[a].pos)/(g[b].pos-g[a].pos);
	return rgbl(t, g[a].color, g[b].color);
}
