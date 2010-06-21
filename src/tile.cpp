#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <agg2/agg_renderer_primitives.h>
#include <agg2/agg_renderer_outline_aa.h>

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

class conv{
	public:
	int ox, oy; // pixel offset (before scaling)
	int sc; // scale level
	double w; // width of a single (pixel at scale 0)
	conv(int tx, int ty, int _sc) : ox(tx*TILE_SIZE*256), oy(ty*TILE_SIZE*256), sc(_sc), w(ldexp(1,_sc)) {}
	int x(int v) {return ((v*256<<sc)-ox);}
	int y(int v) {return ((v*256<<sc)-oy);}
};

void tile(int tx, int ty, int sc, rendererbase& rbase) {
	conv c(tx,ty,sc);
	rbase.clear(agg::rgba8(255, 255, 255));
	
	agg::line_profile_aa lp(c.w,agg::gamma_none());
	agg::renderer_outline_aa<rendererbase> aa(rbase,lp);
	agg::line_parameters line(c.x(-50),c.y(400),c.x(600),c.y(0),256*256<<sc);
	aa.color(agg::rgba8(0,0,0));
	aa.line0(line);
}

void initialize() {
	
}

/*{*/}
