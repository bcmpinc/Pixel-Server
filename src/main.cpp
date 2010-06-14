#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sstream>

typedef unsigned long long int uint64_t;
#include <microhttpd.h>

#include <png.h>

using namespace std;

#define PORT 9999
#define TILE_SIZE 256


#define HEAD(title) \
	"<html><head><title>" title "</title>" \
	"<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />" \
	"</head><body>\n"
#define TAIL() \
	"</body></html>\n"

// Uses the content of the stream as the response.
int dumpstream(struct MHD_Connection * connection, stringstream &s, int status) {
	struct MHD_Response * response = MHD_create_response_from_data(s.tellp(), (void*)s.str().c_str(), MHD_NO, MHD_YES);
	int ret = MHD_queue_response(connection, status, response);
	MHD_destroy_response(response);
	return ret;
}

int dumpimage(struct MHD_Connection * connection) {
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		stringstream s;
		s << HEAD("500 - Error!");
		s << "Could not create png write struct\n";
		s << TAIL();
		return dumpstream(connection, s, MHD_HTTP_INTERNAL_SERVER_ERROR);
	}
	struct MHD_Response * response = MHD_create_response_from_data(0, (void*)0, MHD_NO, MHD_YES);
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
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
	if (0 != strcmp(method, "GET")) return MHD_NO; /* unexpected method */
	stringstream s;
	if (0 == strcmp(url, "/")) {
		s << HEAD("Pixel-Server");
		s << "<h1>Pixel-Server</h1>\n";
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 6; x++) {
				s << "<img src=\"tile?x=" << x << "&y=" << y << "\"/>";
			}
			s << "<br/>\n";
		}
		s << TAIL();
	} else if (0 == strcmp(url, "/style.css")) {
		s << "img {width: " << TILE_SIZE << "; height: " << TILE_SIZE << "; }\n";
	} else if (0 == strcmp(url, "/tile")) {
		
	} else {
		s << HEAD("404 - Not Found!");
		s << "Could not find " << url << endl;
		s << TAIL();
		status = MHD_HTTP_NOT_FOUND;
	}
	return dumpstream(connection, s, status);
}

void * my_logger(void * cls, const char * uri) {
	printf("Requested: %s\n", uri);
	return NULL;
}

int main(int argc, char ** argv) {
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
	while(getchar());
	return 0;
}
