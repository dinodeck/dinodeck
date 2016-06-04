#ifndef FIELD_H
#define FIELD_H

#include <sstream>
#include <string>

#include "Reflect.h"

class Field
{
public:
    virtual ~Field() {}
    const char* Name() const { return mName; }
    // This could be reflect ... probably!
    virtual std::string ToString(void* instance) const = 0;
    virtual void* GetPtr(void* instance) { return NULL; }
    virtual void SetPtr(void* instance, void* value) {}
protected:
    Field(const char* name)  :
        mName(name) {}
private:
    const char* mName;
};


template<typename CLASS, typename TYPE>
class FieldImpl : public Field
{
private:
    // This points to the field, given an instance object
    TYPE CLASS::* mPointer;
public:
    FieldImpl(const char* name,  TYPE CLASS::* pointer):
        Field(name),
        mPointer(pointer)
        {}


    virtual void* GetPtr(void* instance)
    {
        CLASS* typedInstance = static_cast<CLASS*>(instance);
        return (void*)((*typedInstance).*mPointer);
    }

    virtual void SetPtr(void* instance, void* value)
    {
        CLASS* typedInstance = static_cast<CLASS*>(instance);
        ((*typedInstance).*mPointer) = (TYPE)value;
    }

    virtual std::string ToString(void* instance) const
    {
        // Throwing away the type can be avoided
        // but this works as proof of concept.
        CLASS* typedInstance = (CLASS*) instance;
        std::stringstream ss;
        ss << "[" << Name() << "]: "
        << (*typedInstance).*mPointer;
        return ss.str();
    }

};

#endif