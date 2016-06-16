#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "DinodeckGL.h"

class FrameBuffer
{
	GLuint mBufferId;
	GLuint mTextureId;
public:
	FrameBuffer() :
		mBufferId(0),
		mTextureId(0)
		{}
	~FrameBuffer() { DestroyBuffer(); }
	void Reset(unsigned width, unsigned height);

	void Enable();
	void Disable();
	int TextureId() const { return mTextureId; }
private:
	void DestroyBuffer();
};

#endif
