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
	{0.16, rgb(128,128, 64)},
	{0.18, rgb(255,192,  0)},
	{0.20, rgb(  0,255,  0)},
	{0.25, rgb(  0,192,  0)},
	{0.35, rgb(  0,192,  0)},
	{0.39, rgb(192,128, 64)},
	{0.40, rgb(128,128,128)},
	{0.45, rgb(192,192,192)},
	{0.50, rgb(255,255,255)},
};

struct square {
	float aa;
	float ab;
	float ba;
	float bb;
};

void subdiv(square &s, bool x, bool y) {
	if (x) { // right
		s.aa=s.ba/2+s.aa/2;
		s.ab=s.bb/2+s.ab/2;
	} else { // left
		s.ba=s.aa/2+s.ba/2;
		s.bb=s.ab/2+s.bb/2;
	}
	if (y) { // bottom
		s.aa=s.ab/2+s.aa/2;
		s.ba=s.bb/2+s.ba/2;
	} else { // top
		s.ab=s.aa/2+s.ab/2;
		s.bb=s.ba/2+s.bb/2;
	}
}

inline void addnoise_internal(float &v, uint x, uint y, int sc) {
	v+=(noise::noise(x,y,sc)-v)*ldexp(4,-sc);
}

void addnoise(square &s, uint x, uint y, int sc) {
	addnoise_internal(s.aa, x, y  , sc);
	addnoise_internal(s.ab, x, y+1, sc);
	addnoise_internal(s.ba, x+1, y  , sc);
	addnoise_internal(s.bb, x+1, y+1, sc);
}

/**
 * A tile with given coordinates and scale.
 */
square basetile(uint x, uint y, int sc) {
	square s;
	if (sc<=1) {
		s.aa=noise::noise(x  ,y  ,0);
		s.ab=noise::noise(x  ,y+1,0);
		s.ba=noise::noise(x+1,y  ,0);
		s.bb=noise::noise(x+1,y+1,0);
	} else {
		s=basetile(x/2,y/2,sc-1);
		subdiv(s, x&1, y&1);
		addnoise(s, x, y, sc);
	}
	return s;
}

void b(int x, int y, square s, int sc, int * p, int d) {
	
}

void tile(int tx, int ty, int sc, int * p) {
	sc+=4;
	if (sc<0) printf("Scale is negative.\n");
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			square s=basetile(tx*TILE_SIZE+x,ty*TILE_SIZE+y,sc+8);
			p[x+y*TILE_SIZE] = grad(g, LENGTHOF(g), s.aa);
		}
	}
}

void initialize() {
	
}

/*{*/}
