#include <png.h>
#include "pngwrite.h"


namespace pngwrite {/*}*/

void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	std::stringstream * s = (std::stringstream *)png_get_io_ptr(png_ptr);
	s->write((const char*)data, length);
}
void user_flush_data(png_structp png_ptr) {
}

bool writeimage(int width, int height, int * data, std::stringstream &s) {
	bool ret = false;
	png_bytep row_pointers[height];
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (png_ptr && info_ptr && !setjmp(png_jmpbuf(png_ptr))) {
		png_set_write_fn(png_ptr, &s, user_write_data, user_flush_data);
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		for (int i = 0; i < height; i++) row_pointers[i] = (png_byte*)(data + i*width);
		png_set_rows(png_ptr, info_ptr, row_pointers);
		png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);
		ret = true;
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return ret;
}

/*{*/}
