#ifndef ISCREENCHANGELISTENER_H
#define ISCREENCHANGELISTENER_H

class IScreenChangeListener
{
public:
    virtual void OnChange(int width, int height) = 0;
    virtual ~IScreenChangeListener() {};
};

#endif