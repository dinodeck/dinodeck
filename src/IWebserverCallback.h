#ifndef IWEBSERVERCALLBACK_H
#define IWEBSERVERCALLBACK_H


class IWebServerCallback
{
public:
    virtual std::string OnWebRequest(const std::string& uri, const std::string& postdata) = 0;
    virtual ~IWebServerCallback() {};
};

#endif