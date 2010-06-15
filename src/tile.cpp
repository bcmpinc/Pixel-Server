#include "tile.h"
#include "noise.h"
#include <cmath>

namespace tile {/*}*/

// Color functions.
#define CLAMP(x) (x<0?0:x>255?255:x)
inline int rgb(int r, int g, int b) {
	return CLAMP(r)|CLAMP(g)<<8|CLAMP(b)<<16;
}
inline int rgb(float r, float g, float b) {
	return rgb(fist(r*255),fist(g*255),fist(b*255));
}

float fs(float x, float y, float sc) {
	float w=.5;
	float r=0;
	x*=sc;
	y*=sc;
	for (int i=0; i<12; i++) {
		w*=.7;
		r+=w*noise::noise(x, y);
		x+=y;
		y*=2;
		y-=x;
		x+=5;
	}
	return sin(15*r);
}

int grad(float f) {
	f=.5-f/2;
	if (f<0.2f) return 0xffffff;
	if (f>0.8f) return 0x000000;
	if (f<0.4f) return rgb(1.2f-f,1.2f-f,1.6f-3*f);
	if (f<0.6f) return rgb(1.2f-f,2.0f-3*f,1.6f-3*f);
	return rgb(2.4f-3*f,2.0f-3*f,1.6f-3*f);
}

void tile(int tx, int ty, float sc, int * p) {
	sc/=10000;
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			p[x+y*TILE_SIZE] = grad(fs((tx*TILE_SIZE+x), (ty*TILE_SIZE+y), sc));
		}
	}
}

void initialize() {
	
}

/*{*/}
