#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "tile.h"
#include "noise.h"
#include "rgb.h"

namespace tile {/*}*/

#define LENGTHOF(a) ((sizeof(a)/sizeof(a[0])))
Gradient h[] = {
	{ -1, rgb(  0,  0,  0)},
	{ -1, rgb(255,  0,  0)},
	{-.5, rgb(255,  0,255)},
	{  0, rgb(255,255,255)},
	{ .5, rgb(  0,255,255)},
	{  1, rgb(  0,255,  0)},
	{  1, rgb(  0,  0,  0)},
};
Gradient g[] = {
	{0.10, rgb(  0,  0,192)},
	{0.15, rgb(  0,255,255)},
	{0.151, rgb(128,128, 64)},
	{0.18, rgb(255,192,  0)},
	{0.20, rgb(  0,255,  0)},
	{0.25, rgb(  0,192,  0)},
	{0.35, rgb(  0,192,  0)},
	{0.399, rgb(192,128, 64)},
	{0.40, rgb(128,128,128)},
	{0.45, rgb(192,192,192)},
	{0.50, rgb(255,255,255)},
};

float fs(float x, float y, float sc) {
	float w=1;
	float r=0;
	x*=sc;
	y*=sc;
	for (int i=0; i<12; i++) {
		w*=.5;
		r+=w*noise::noise(x, y);
		y*=2;
		x*=2;
	}
	return r;
}

void tile(int tx, int ty, int sc, int * p) {
	sc+=14;
	if (sc<0) printf("Scale is negative.\n");
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			p[x+y*TILE_SIZE] = grad(g, LENGTHOF(g), fs(x+tx*TILE_SIZE,y+ty*TILE_SIZE,ldexp(1,-sc)));
		}
	}
}

void initialize() {
	
}

/*{*/}
