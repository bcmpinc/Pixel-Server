#include "tile.h"
#include "noise.h"

namespace tile {/*}*/

// Color functions.
#define CLAMP(x) (x<0?0:x>255?255:x)
inline int rgb(int r, int g, int b) {
	return CLAMP(r)|CLAMP(g)<<8|CLAMP(b)<<16;
}
inline int rgb(float r, float g, float b) {
	return rgb(fist(r*255),fist(g*255),fist(b*255));
}

float f(float x, float y) {
	return noise::noise(x, y);
}

float fs(float x, float y) {
	return f(x/64,y/64)*0.5+f(x/37,y/37)*0.3+f(x/17,y/17)*0.14+f(x/7,y/7)*0.06;
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
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			p[x+y*TILE_SIZE] = grad(fs(sc*(tx*TILE_SIZE+x), sc*(ty*TILE_SIZE+y)));
		}
	}
}

void initialize() {
	
}

/*{*/}
