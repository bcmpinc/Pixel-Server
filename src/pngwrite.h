#ifndef PNGWRITE_H
#define PNGWRITE_H

#include <sstream>

namespace pngwrite {
	bool writeimage(int width, int height, int * data, std::stringstream &s);
}

#endif
