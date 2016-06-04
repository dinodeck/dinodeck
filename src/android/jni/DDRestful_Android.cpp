#include "../../DDRestful.h"


#include "../../HttpPostData.h"
#include "../../LuaState.h"
#include "AndroidWrapper.h"
#include "DDLuaCallbacks.h"

int DDRestful::Post(const char* uri, HttpPostData* postData,
                    int successRef,
                    int failureRef)
{
    int callbackId = DDLuaCallbacks::StoreCallback(successRef, failureRef);
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->HttpPost(uri, postData, callbackId);
    return callbackId;
}
