#include <cstdlib>
#include "tile.h"

inline int fist(float f) {
	int r;
	__asm__ __volatile__ ("fistpl %0" : "=m" (r) : "t" (f) : "st") ;
	return r;
}

// Color functions.
#define CLAMP(x) (x<0?0:x>255?255:x)
inline int rgb(int r, int g, int b) {
	return CLAMP(r)|CLAMP(g)<<8|CLAMP(b)<<16;
}
inline int rgb(double r, double g, double b) {
	return rgb(fist(r*255),fist(g*255),fist(b*255));
}

// Noise functions
int p[256];

void tile_init(int argc, char ** argv) {
	srand(1337);
	for (int i=0; i<256; i++) {
		p[i]=i;
	}
	for (int i=0; i<256; i++) {
		int j = rand()&0xff;
		p[i]^=p[j]^=p[i]^=p[j];
	}
}

float noise(int x, int y) {
	return p[(x+p[y&0xff])&0xff]/255.0f;
}

float lerp(float t, float a, float b) {
	return a*(1-t)+b*t;
}

float noise(float x, float y) {
	int px = fist(x);
	int py = fist(y);
	if (x<px) px--;
	if (y<py) py--;
	x-=px;
	y-=py;
	return lerp(y,
		lerp(x,noise(px,py),  noise(px+1,py)),
		lerp(x,noise(px,py+1),noise(px+1,py+1)));
}

float f(float x, float y) {
	return .3/noise(x,y);
}

float fs(float x, float y) {
	return f(x/64,y/64)*0.5+f(x/37,y/37)*0.3+f(x/17,y/17)*0.14+f(x/7,y/7)*0.06;
}

int grad(float f) {
	if (f<0.2f) return 0xffffff;
	if (f>0.8f) return 0x000000;
	if (f<0.4f) return rgb(1.2-f,1.2-f,1.6-3*f);
	if (f<0.6f) return rgb(1.2-f,2.0-3*f,1.6-3*f);
	return rgb(2.4-3*f,2.0-3*f,1.6-3*f);
}

void tile(int tx, int ty, float sc, int * p) {
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			p[x+y*TILE_SIZE] = grad(fs(sc*(tx*TILE_SIZE+x), sc*(ty*TILE_SIZE+y)));
		}
	}
}
