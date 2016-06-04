#ifndef DDRESTFUL_H
#define DDRESTFUL_H

class HttpPostData;

class DDRestful
{
    public:
        static int Post(const char* uri, HttpPostData* postData,
                     int successRef, int failureRef);
};

#endif