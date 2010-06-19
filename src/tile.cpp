#include "tile.h"
#include "noise.h"
#include "rgb.h"
#include <cmath>

namespace tile {/*}*/

float fs(float x, float y, float sc) {
	float w=2;
	float r=0;
	x*=sc;
	y*=sc;
	for (int i=0; i<8; i++) {
		w*=.5;
		r+=w*noise::noise(x, y);
		y*=2;
		x*=2;
	}
	return r;
}

#define LENGTHOF(a) ((sizeof(a)/sizeof(a[0])))
Gradient g[] = {
	{ -1, rgb(  0,  0,  0)},
	{ -1, rgb(255,  0,  0)},
	{-.5, rgb(255,  0,255)},
	{  0, rgb(255,255,255)},
	{ .5, rgb(  0,255,255)},
	{  1, rgb(  0,255,  0)},
	{  1, rgb(  0,  0,  0)},
};
Gradient h[] = {
	{0.10, rgb(  0,  0,192)},
	{0.15, rgb(  0,255,255)},
	{0.16, rgb(128,128,64)},
	{0.18, rgb(255,192,0)},
	{0.20, rgb(0,255,0)},
	{0.25, rgb(0,192,0)},
	{0.35, rgb(0,192,0)},
	{0.39, rgb(192,128,64)},
	{0.40, rgb(128,128,128)},
	{0.45, rgb(192,192,192)},
	{0.50, rgb(255,255,255)},
};

void tile(int tx, int ty, float sc, int * p) {
	sc/=16384;
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			p[x+y*TILE_SIZE] = grad(g, LENGTHOF(g), fs((tx*TILE_SIZE+x), (ty*TILE_SIZE+y), sc));
		}
	}
}

void initialize() {
	
}

/*{*/}
