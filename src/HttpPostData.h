#ifndef HTTPPOSTDATA_H
#define HTTPPOSTDATA_H

#include <string>
#include <map>

#include "reflect/Reflect.h"

class LuaState;

class HttpPostData
{

    public: static Reflect Meta;
    std::map<std::string, std::string> mValueMap;
    public:
        enum ContentType
        {
            //http://www.brandonchecketts.com/archives/array-versus-string-in-curlopt_postfields
            Application, // "application/x-www-form-urlencoded"
            Multipart   // "multipart/form-data"
        };
        static void Bind(LuaState* state);

        HttpPostData(ContentType contentType);
        HttpPostData(const HttpPostData& httpPostData);
        HttpPostData();
        ~HttpPostData();
        bool AddValue(const char* key, const char* value);
        const std::map<std::string, std::string>& GetValues() const { return mValueMap; };
};

#endif