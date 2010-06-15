#ifndef TILE_H
#define TILE_H

#define TILE_SIZE 128
#define TILES_X 10
#define TILES_Y 5

namespace tile {
	void initialize();
	void tile(int x, int y, float scale, int * pixels);
}

#endif
