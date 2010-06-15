#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "httpserv.h"

void Connection::head(const char * title) throw() {
	s<<"<html><head><title>"<<title<<"</title>\n";
	s<<"<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />";
	s<<"</head><body>\n";
}

void Connection::tail() throw() {
	s<<"</body></html>\n";
}

void append_headers(struct MHD_Response * response, HttpHeader * headers) throw() {
	if (headers) while(headers->key) { 
		MHD_add_response_header(response, headers->key, headers->value);
		headers++;
	}
}

// Uses the content of the stream as the response.
int Connection::dumpstream(int status, const char * mimetype, HttpHeader * headers) throw() {
	struct MHD_Response * response = MHD_create_response_from_data(s.tellp(), (void*)s.str().c_str(), MHD_NO, MHD_YES);
	append_headers(response, server->common_headers);
	append_headers(response, headers);
	int ret = MHD_queue_response(connection, status, response);
	MHD_destroy_response(response);
	return ret;
}

void Connection::invalidarg(const char * arg, const char * type, const char * state) throw(int) {
	printf("Invalid arg: %s\n", arg);
	head("422 - Argument missing");
	s<<"Argument "<<arg<<" = ["<<type<<"] is "<<state<<".\n";
	tail();
	throw dumpstream(MHD_HTTP_UNPROCESSABLE_ENTITY, "text/html");
}

int Connection::getint(MHD_ValueKind kind, const char * arg) throw(int) {
	int result;
	const char * value = MHD_lookup_connection_value(connection, kind, arg);
	if (!value) invalidarg(arg, "integer", "missing");
	else if (sscanf(value, "%d", &result)<0) invalidarg(arg, "integer", "invalid");
	return result;
}

int Connection::getint(MHD_ValueKind kind, const char * arg, int def) throw(int) {
	int result=def;
	const char * value = MHD_lookup_connection_value(connection, kind, arg);
	if (value && sscanf(value, "%d", &result)<0) invalidarg(arg, "integer", "invalid");
	return result;
}

float Connection::getfloat(MHD_ValueKind kind, const char * arg) throw(int) {
	float result;
	const char * value = MHD_lookup_connection_value(connection, kind, arg);
	if (!value) invalidarg(arg, "float", "missing");
	else if (sscanf(value, "%f", &result)<0) invalidarg(arg, "float", "invalid");
	return result;
}

float Connection::getfloat(MHD_ValueKind kind, const char * arg, float def) throw(int) {
	float result = def;
	const char * value = MHD_lookup_connection_value(connection, kind, arg);
	if (value && sscanf(value, "%f", &result)<0) invalidarg(arg, "float", "invalid");
	return result;
}


const char * Connection::getstring(MHD_ValueKind kind, const char * arg) throw(int) {
	const char * result = MHD_lookup_connection_value(connection, kind, arg);
	if (!result) invalidarg(arg, "float", "missing");
	return result;
}

const char * Connection::getstring(MHD_ValueKind kind, const char * arg, const char * def) throw(int) {
	const char * result = MHD_lookup_connection_value(connection, kind, arg);
	if (!result) return def;
	return result;
}

bool Connection::isurl(const char * u) throw() {
	return 0 == strcmp(u, url);
}

Connection::Connection(const char * u, struct MHD_Connection * c, class HttpServer * s) throw() :
	url(u), connection(c), server(s) {
}

int http_handler(void * cls,
			struct MHD_Connection * connection,
			const char * url,
			const char * method,
			const char * version,
			const char * upload_data,
			size_t * upload_data_size,
			void ** ptr) throw()
{
	if (0 != strcmp(method, "GET")) return MHD_NO; /* unexpected method */
	try {
		HttpServer * serv = (HttpServer*)cls;
		Connection c(url, connection, serv);
		return serv->handler(c);
	} catch (int ret) {
		return ret;
	}
}

void * my_logger(void * cls, const char * uri) throw() {
	printf("Requested: %s - ", uri);
	return NULL;
}

HttpServer::HttpServer(HttpHandler h, int port, HttpHeader * ch) throw(int) : 
	common_headers(ch), handler(h) {
	printf("Creating daemon.\n");
	daemon = MHD_start_daemon(
		MHD_USE_THREAD_PER_CONNECTION,
		port,
		NULL,
		NULL,
		&http_handler,
		this,
		MHD_OPTION_URI_LOG_CALLBACK,
		my_logger,
		NULL,
		MHD_OPTION_END
	);
	if (daemon == NULL){
		printf("Could not create daemon.\n");
		throw(1);
	}
	printf("Daemon created.\n");
}

