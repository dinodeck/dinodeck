#ifndef SHADER_H
#define SHADER_H

#include "IResourceLoader.h"

class Resource;

class Shader : IResourceLoader
{


private:
    CGcontext mContext;

    // Each program may have a different
    // vertex format or globals
    // The resource is probably going to
    // have to be some kind lua file
    //
    // {
    //    program = "some.cg",
    //    paramters =
    //    {
    //       "vIn.position",
    //       "modelViewProj",
    //    }
    // }
    //
    struct Program
    {
        CGprogram shader;
        std::map<std::string, CGparameter> parameters;
    };

    // string -> program, loaded using reload
    std::map<std::string, Program> mShaders;

    // Used to determine if shader can run
    CGprofile mVertexProfile;
    CGprofile mFragmentProfile;
public:
    // Setup all the shader stuff.
    void Init();
    virtual bool Reload(Resource& resource);
};


#endif