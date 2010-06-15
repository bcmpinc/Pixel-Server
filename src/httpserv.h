#ifndef HTTPSERV_H
#define HTTPSERV_H

#include <sstream>
typedef unsigned long long int uint64_t;
#include <microhttpd.h>

struct HttpHeader {
	const char * key;
	const char * value;
};

class Connection {
public:
	Connection(const char * url, struct MHD_Connection *, class HttpServer *) throw();
	void head(const char * title) throw();
	void tail() throw();

	int dumpstream(int status, const char * mimetype, HttpHeader * headers = NULL) throw();
	
	void invalidarg(const char * arg, const char * type, const char * state) throw(int);
	int getint(MHD_ValueKind kind, const char * arg) throw(int);
	int getint(MHD_ValueKind kind, const char * arg, int def) throw(int);
	float getfloat(MHD_ValueKind kind, const char * arg) throw(int);
	float getfloat(MHD_ValueKind kind, const char * arg, float def) throw(int);
	const char * getstring(MHD_ValueKind kind, const char * arg) throw(int);
	const char * getstring(MHD_ValueKind kind, const char * arg, const char * def) throw(int);

	std::stringstream s;
	const char * url;
	
	bool isurl(const char *) throw();
private:
	struct MHD_Connection * connection;
	class HttpServer * server;
};

typedef int (*HttpHandler)(Connection&);
class HttpServer {
public:
	HttpServer(HttpHandler h, int port, HttpHeader* common_headers) throw(int);
	HttpHeader * common_headers;
	const HttpHandler handler;
	
private:
	struct MHD_Daemon * daemon;
};

#endif
