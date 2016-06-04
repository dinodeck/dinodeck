#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <queue>
#include <string>
#include <mongoose.h>

class IWebServerCallback;

//
// Creates a HttpServer to listen on a port for incoming requests.
// If there's a request pending PopRequest will return true and fill in the HttpRequest struct.
//
class WebServer
{
private:
    mg_context* mContext;
    static void* callback(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info);
public:
    IWebServerCallback* mCallback;
    WebServer(int port, IWebServerCallback* callback);
    ~WebServer();
};

#endif