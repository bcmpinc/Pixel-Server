#ifndef TILE_H
#define TILE_H

#define TILE_SIZE 256
#define TILES_X 5
#define TILES_Y 3

namespace tile {
	void initialize();
	void tile(int x, int y, float scale, int * pixels);
}

#endif
