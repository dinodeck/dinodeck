#ifndef IASSETOWDNER_H
#define IASSETOWDNER_H

#include <stdio.h>
class Asset;

class IAssetOwner
{
public:
	virtual bool OnAssetReload(Asset& asset) = 0;
    virtual void OnAssetDestroyed(Asset& asset) = 0;
	virtual ~IAssetOwner() {};
};

#endif