#include "tile.h"
#include "noise.h"
#include "float.h"
#include <cmath>

namespace tile {/*}*/

// Color functions.
#define CLAMP(x) (x<0?0:x>255?255:x)
inline int rgb(int r, int g, int b) {
	return CLAMP(r)|CLAMP(g)<<8|CLAMP(b)<<16;
}
inline int rgb(float r, float g, float b) {
	return rgb(fist(r),fist(g),fist(b));
}
inline int rgbu(float r, float g, float b) {
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

float fs(float x, float y, float sc) {
	float w=1;
	float r=0;
	x*=sc;
	y*=sc;
	for (int i=0; i<8; i++) {
		w*=.5;
		r+=w*noise::noise(x, y);
		x+=y;
		y*=2.82427;
		x*=1.41214;
		y-=x;
		x+=5;
	}
	return r-0.1;
}

int grad(float f) {
	if (f< 0.00f) return rgb(0,0,192);
	if (f< 0.10f) return rgbl((f+0.00)/.10, rgb(0,0,192), rgb(0,255,255));
	if (f< 0.15f) return rgbl((f-0.10)/.05, rgb(128,128,64), rgb(255,192,0));
	if (f< 0.25f) return rgbl((f-0.15)/.10, rgb(255,192,0), rgb(0,255,0));
	if (f< 0.50f) return rgbl((f-0.25)/.25, rgb(0,255,0), rgb(0,128,0));
	if (f< 0.65f) return rgbl((f-0.50)/.05, rgb(0,128,0), rgb(192,192,192));
	return rgb(192,192,192);
}

void tile(int tx, int ty, float sc, int * p) {
	sc/=8192;
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			p[x+y*TILE_SIZE] = grad(fs((tx*TILE_SIZE+x), (ty*TILE_SIZE+y), sc));
		}
	}
}

void initialize() {
	
}

/*{*/}
