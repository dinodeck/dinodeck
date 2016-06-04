#include "Webserver.h"

#include <assert.h>
#include <mongoose.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "DDLog.h"
#include "IWebserverCallback.h"

#define MAX_PORT_NUMBER 65535

#if __APPLE__
int itoa(int value, char *buf, size_t sz)
{
    return snprintf(buf, sz, "%d", value);
}
#endif

void* WebServer::callback(enum mg_event event,
                          struct mg_connection* conn,
                          const struct mg_request_info* request_info)
{
    if (event == MG_NEW_REQUEST)
    {
        assert(request_info->user_data);
        WebServer* server = static_cast<WebServer*>(request_info->user_data);
        // Me
        const char* cl = mg_get_header(conn, "Content-Length");
        std::string postdata;
        if(!strcmp(request_info->request_method, "POST")
            && cl != NULL)
        {
            // handle post data
            //printf("Content-length:%s\n", cl);
            //printf("Content-length%d\n", conn->content_len);
            size_t buf_len = atoi(cl);
            char* buffer = (char*) malloc(buf_len + 1);
            mg_read(conn, buffer, buf_len);
            buffer[buf_len] = '\0';
           // printf("%s\n", buffer);
            postdata = buffer;
            free(buffer);
        }

        //dsprintf("Webserver message: %s", request_info->uri);
        std::string output = server->mCallback->OnWebRequest
        (
            std::string(request_info->uri),
            std::string(postdata)
        );

        if(output == "")
        {
            mg_printf(conn, "HTTP/1.1 200 OK\r\n\r\n");
            return (void*)"";
        }

        //dsprintf("Sending back webserver data: %s", output.c_str());
        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\n\r\n"
                  "Content-Type: text/plain\r\n"
                  "Content-Length: %d\r\n"        // Always set Content-Length
                  "\r\n"
                  "%s",
                  (int)output.length() + 1, output.c_str());

        // Mark as processed
        return (void*)"";
    }
    else
    {
        return NULL;
    }
}

WebServer::WebServer(int port, IWebServerCallback* callback)
{
    assert(port > 0);
    assert(port <= MAX_PORT_NUMBER);
    char portStrBuffer[64];
    itoa(port, portStrBuffer, 10);

    const char *options[] = {"listening_ports", portStrBuffer, NULL};
    mContext = mg_start(&WebServer::callback, this, options);
    mCallback = callback;
}

WebServer::~WebServer()
{
    mg_stop(mContext);
}