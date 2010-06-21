#ifndef TILE_H
#define TILE_H

#include <agg2/agg_pixfmt_rgba.h>
#include <agg2/agg_renderer_base.h>

#define TILE_SIZE 256
#define TILES_X 2
#define TILES_Y 1

namespace tile {
	typedef agg::pixfmt_rgba32 pixfmt;
	typedef agg::renderer_base<pixfmt> rendererbase;
	
	void initialize();
	void tile(int x, int y, int scale, rendererbase& pixels);
}

#endif
