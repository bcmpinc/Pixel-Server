#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sstream>

typedef unsigned long long int uint64_t;
#include <microhttpd.h>

#include "tile.h"
#include "init.h"
#include "pngwrite.h"

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

void invalidarg(struct MHD_Connection * connection, const char * arg, const char * type, const char * state) throw(int) {
	printf("Invalid arg: %s\n", arg);
	stringstream s;
	s<<HEAD("422 - Argument missing")<<"Argument "<<arg<<" = ["<<type<<"] is "<<state<<".\n"<<TAIL();
	throw dumpstream(connection, s, MHD_HTTP_UNPROCESSABLE_ENTITY, "text/html");
}

int getint(struct MHD_Connection * connection, const char * arg) {
	int result;
	const char * value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, arg);
	if (!value) {
		invalidarg(connection, arg, "integer", "missing");
	} else if (sscanf(value, "%d", &result)<0)
		invalidarg(connection, arg, "integer", "invalid");
	return result;
}

int getint(struct MHD_Connection * connection, const char * arg, int def) {
	int result=def;
	const char * value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, arg);
	if (value && sscanf(value, "%d", &result)<0)
		invalidarg(connection, arg, "integer", "invalid");
	return result;
}

float getfloat(struct MHD_Connection * connection, const char * arg) {
	float result;
	const char * value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, arg);
	if (!value) {
		invalidarg(connection, arg, "float", "missing");
	} else if (sscanf(value, "%f", &result)<0)
		invalidarg(connection, arg, "float", "invalid");
	return result;
}

float getfloat(struct MHD_Connection * connection, const char * arg, float def) {
	float result = def;
	const char * value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, arg);
	if (value && sscanf(value, "%f", &result)<0)
		invalidarg(connection, arg, "float", "invalid");
	return result;
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

	try {
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
			int x = getint(connection, "x");;
			int y = getint(connection, "y");;
			float scale = getfloat(connection, "scale", 1.0);;
			
			printf("Creating image\n");
			int * pixels = new int[TILE_SIZE * TILE_SIZE];
			tile::tile(x, y, scale, pixels);
			for (int i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
				pixels[i] |= 0xff000000;
			}
			int ret;
			if (pngwrite::writeimage(TILE_SIZE, TILE_SIZE, pixels, s)) {
				ret = dumpstream(connection, s, MHD_HTTP_OK, "image/png");
			} else {
				s << HEAD("500 - Error!");
				s << "Could not create png image.\n";
				s << TAIL();
				ret = dumpstream(connection, s, MHD_HTTP_INTERNAL_SERVER_ERROR, "text/html");
			}
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
	} catch (int ret) {
		return ret;
	}
}

void * my_logger(void * cls, const char * uri) {
	printf("Requested: %s - ", uri);
	return NULL;
}

int main(int argc, char ** argv) {
	printf("Initializing.\n");
	initialize();
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
