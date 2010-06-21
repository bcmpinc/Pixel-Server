#ifndef PNGWRITE_H
#define PNGWRITE_H

#include <sstream>
#include <agg2/agg_rendering_buffer.h>


namespace pngwrite {
	/** Writes the buffer to the stream, png encoded, assuming the buffer is in rgba format. */
	bool writeimage(agg::rendering_buffer &rbuf, std::stringstream &s);
}

#endif
