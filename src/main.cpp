#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "tile.h"
#include "init.h"
#include "pngwrite.h"
#include "httpserv.h"

#define PORT 9999

char etag[32];

int handler(Connection& c) {
	if (0 == strcmp(c.getstring(MHD_HEADER_KIND, MHD_HTTP_HEADER_IF_NONE_MATCH, ""), etag)) {
		printf("Not modified\n");
		return c.dumpstream(MHD_HTTP_NOT_MODIFIED, "text/html");
	} else if (c.isurl("/")) {
		printf("Page\n");
		int px = c.getint(MHD_GET_ARGUMENT_KIND, "x", 0);
		int py = c.getint(MHD_GET_ARGUMENT_KIND, "y", 0);
		float scale = c.getfloat(MHD_GET_ARGUMENT_KIND, "scale", 1.0);
		c.head("Pixel-Server");
		c.s << "<h2>Pixel-Server</h2>\n";
		c.s << "<a href=\"?x="<<px<<"&y="<<(py-1)<<"&scale="<<scale<<"\"class=\"ya\">^</a>";
		c.s << "<a href=\"?x="<<px<<"&y="<<(py+1)<<"&scale="<<scale<<"\"class=\"yb\">v</a>";
		c.s << "<a href=\"?x="<<(px-1)<<"&y="<<py<<"&scale="<<scale<<"\"class=\"xa\">&lt;</a>";
		c.s << "<a href=\"?x="<<(px+1)<<"&y="<<py<<"&scale="<<scale<<"\"class=\"xb\">&gt;</a>";
		c.s << "<a href=\"?x="<<(px/2)<<"&y="<<(py/2)<<"&scale="<<(scale*2)<<"\"class=\"zo\">-</a>";
		c.s << "<table>\n";
		for (int y = -TILES_Y; y <= TILES_Y; y++) {
			c.s << "<tr>";
			for (int x = -TILES_X; x <= TILES_X; x++) {
				c.s << "<td><a href=\"?scale="<<(scale/2)<<"&x="<<(px*2+x*2)<<"&y="<<(py*2+y*2)<<"\">";
				c.s << "<img src=\"tile?x=" << (px+x) << "&y=" << (py+y);
				if (scale != 1.0) c.s << "&scale=" << scale;
				c.s << "\"/></a></td>";
			}
			c.s << "</tr>\n";
		}
		c.s << "</table>\n";
		c.tail();
		return c.dumpstream(MHD_HTTP_OK, "text/html");
	} else if (c.isurl("/style.css")) {
		printf("Style sheet\n");
		c.s << "img {width: " << TILE_SIZE << "; height: " << TILE_SIZE << "; border: none; }\n";
		c.s << "table {border-spacing: 0; border: 1px solid black;}\n";
		c.s << "td {padding: 0; }\n";
		c.s << "h2 {position: absolute; background: rgba(255,255,255,0.5);}\n";
		c.s << ".xa, .xb, .ya, .yb, .zo {position: absolute; display: block; width: 18px; height: 18px; border:1px solid black; text-align: center; color: black; text-decoration: none;}\n";
		c.s << ".xa {top: 25px; right: 45px;}\n";
		c.s << ".xb {top: 25px; right:  5px;}\n";
		c.s << ".ya {top:  5px; right: 25px;}\n";
		c.s << ".yb {top: 45px; right: 25px;}\n";
		c.s << ".zo {top: 25px; right: 25px;}\n";
		return c.dumpstream(MHD_HTTP_OK, "text/css");
	} else if (c.isurl("/tile")) {
		int x = c.getint(MHD_GET_ARGUMENT_KIND, "x");
		int y = c.getint(MHD_GET_ARGUMENT_KIND, "y");
		float scale = c.getfloat(MHD_GET_ARGUMENT_KIND, "scale", 1.0);
		
		printf("Creating image\n");
		int * pixels = new int[TILE_SIZE * TILE_SIZE];
		tile::tile(x, y, scale, pixels);
		for (int i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
			pixels[i] |= 0xff000000;
		}
		int ret;
		if (pngwrite::writeimage(TILE_SIZE, TILE_SIZE, pixels, c.s)) {
			ret = c.dumpstream(MHD_HTTP_OK, "image/png");
		} else {
			c.head("500 - Error!");
			c.s << "Could not create png image.\n";
			c.tail();
			ret = c.dumpstream(MHD_HTTP_INTERNAL_SERVER_ERROR, "text/html");
		}
		delete[] pixels;
		return ret;
	} else {
		printf("Not found\n");
		c.head("404 - Not Found!");
		c.s << "Could not find " << c.url << std::endl;
		c.tail();
		return c.dumpstream(MHD_HTTP_NOT_FOUND, "text/html");
	}
}


int main(int argc, char ** argv) {
	printf("Initializing.\n");
	initialize();
	sprintf(etag, "%0lx", time(NULL));
	HttpHeader default_headers[] = {{MHD_HTTP_HEADER_SERVER, "Pixel-Server"}, {MHD_HTTP_HEADER_ETAG, etag}, {NULL}};
	HttpServer(handler, PORT, default_headers);
	
	// Wait indefinitely
	// Except when not connected to a tty.
	while(getchar()>0);
	return 0;
}
