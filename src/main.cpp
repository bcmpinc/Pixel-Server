#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sstream>

typedef unsigned long long int uint64_t;
#include <microhttpd.h>
#include <png.h>

#include "tile.h"

using namespace std;

#define PORT 9999


#define HEAD(title) \
	"<html><head><title>" title "</title>" \
	"<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />" \
	"</head><body>\n"
#define TAIL() \
	"</body></html>\n"

char etag[32];

// Uses the content of the stream as the response.
int dumpstream(struct MHD_Connection * connection, stringstream &s, int status, const char * mimetype) {
	struct MHD_Response * response = MHD_create_response_from_data(s.tellp(), (void*)s.str().c_str(), MHD_NO, MHD_YES);
	MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, mimetype);
	MHD_add_response_header(response, MHD_HTTP_HEADER_ETAG, etag);
	MHD_add_response_header(response, MHD_HTTP_HEADER_SERVER, "Pixel-Server");
	int ret = MHD_queue_response(connection, status, response);
	MHD_destroy_response(response);
	return ret;
}

void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	stringstream * s = (stringstream *)png_get_io_ptr(png_ptr);
	s->write((const char*)data, length);
}
void user_flush_data(png_structp png_ptr) {
}

int dumpimage(struct MHD_Connection * connection, int width, int height, int * data) {
	int ret;
	stringstream s;
	png_bytep row_pointers[height];
	
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (png_ptr && info_ptr && !setjmp(png_jmpbuf(png_ptr))) {
		png_set_write_fn(png_ptr, &s, user_write_data, user_flush_data);
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		for (int i = 0; i < height; i++) row_pointers[i] = (png_byte*)(data + i*width);
		png_set_rows(png_ptr, info_ptr, row_pointers);
		png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);
		ret = dumpstream(connection, s, MHD_HTTP_OK, "image/png");
	} else {
		s << HEAD("500 - Error!");
		s << "Could not create png image.\n";
		s << TAIL();
		ret = dumpstream(connection, s, MHD_HTTP_INTERNAL_SERVER_ERROR, "text/html");
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return ret;
}

int handler(void * cls,
			struct MHD_Connection * connection,
			const char * url,
			const char * method,
			const char * version,
			const char * upload_data,
			size_t * upload_data_size,
			void ** ptr) 
{
	int status = MHD_HTTP_OK;
	const char * mimetype = "text/html";
	if (0 != strcmp(method, "GET")) return MHD_NO; /* unexpected method */

	stringstream s;
	const char * value = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_IF_NONE_MATCH);
	if (value && 0 == strcmp(value, etag)) {
		printf("Not modified\n");
		status = MHD_HTTP_NOT_MODIFIED;
	} else if (0 == strcmp(url, "/")) {
		printf("Page\n");
		value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "scale");
		s << HEAD("Pixel-Server");
		s << "<h1>Pixel-Server</h1>\n";
		s << "<table>\n";
		for (int y = 0; y < TILES_Y; y++) {
			s << "<tr>";
			for (int x = 0; x < TILES_X; x++) {
				s << "<td><img src=\"tile?x=" << x << "&y=" << y;
				if (value) s << "&scale=" << value;
				s << "\"/></td>";
			}
			s << "</tr>\n";
		}
		s << "</table>\n";
		s << TAIL();
	} else if (0 == strcmp(url, "/style.css")) {
		printf("Style sheet\n");
		s << "img {width: " << TILE_SIZE << "; height: " << TILE_SIZE << "; }\n";
		s << "table {border-spacing: 0; border: 1px solid black;}\n";
		s << "td {padding: 0; }\n";
		mimetype = "text/css";
	} else if (0 == strcmp(url, "/tile")) {
		printf("Creating image\n");
		int x;
		int y;
		float scale;
		
		value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "x");
		if (value) sscanf(value, "%d", &x);
		value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "y");
		if (value) sscanf(value, "%d", &y);
		value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "scale");
		if (value) sscanf(value, "%f", &scale);
		else scale = 1;
		
		int * pixels = new int[TILE_SIZE * TILE_SIZE];
		tile(x, y, scale, pixels);
		for (int i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
			pixels[i] |= 0xff000000;
		}
		int ret = dumpimage(connection, TILE_SIZE, TILE_SIZE, pixels);
		delete[] pixels;
		return ret;
	} else {
		printf("Not found\n");
		s << HEAD("404 - Not Found!");
		s << "Could not find " << url << endl;
		s << TAIL();
		status = MHD_HTTP_NOT_FOUND;
	}
	return dumpstream(connection, s, status, mimetype);
}

void * my_logger(void * cls, const char * uri) {
	printf("Requested: %s - ", uri);
	return NULL;
}

int main(int argc, char ** argv) {
	printf("Initializing.\n");
	tile_init(argc, argv);
	sprintf(etag, "%0lx", time(NULL));
	printf("Creating daemon.\n");
	struct MHD_Daemon * d = MHD_start_daemon(
		MHD_USE_THREAD_PER_CONNECTION,
		PORT,
		NULL,
		NULL,
		&handler,
		NULL,
		MHD_OPTION_URI_LOG_CALLBACK,
		my_logger,
		NULL,
		MHD_OPTION_END
	);
	if (d == NULL){
		printf("Could not create daemon.\n");
		return 1;
	}
	printf("Daemon created.\n");
	
	// Wait indefinitely
	// Except when not connected to a tty.
	while(getchar()>0);
	return 0;
}
