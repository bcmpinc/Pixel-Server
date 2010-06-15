#ifndef TILE_H
#define TILE_H

#define TILE_SIZE 256
#define TILES_X 2
#define TILES_Y 1

namespace tile {
	void initialize();
	void tile(int x, int y, float scale, int * pixels);
}

#endif
