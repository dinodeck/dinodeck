#include "FrameBuffer.h"

#include <assert.h>

#include "DinodeckGL.h"
#include "DDLog.h"

const unsigned int NUM_BUFFERS = 1;

void FrameBuffer::DestroyBuffer()
{
	// Only reset the id, if it exists
	if(mBufferId != 0)
	{
		glDeleteFramebuffers(NUM_BUFFERS, &mBufferId);
		mBufferId = 0;
	}

	if(mTextureId != 0)
	{
		glDeleteTextures(NUM_BUFFERS, &mTextureId);
		mTextureId = 0;
	}
}

void FrameBuffer::Reset(unsigned width, unsigned height)
{
	//dsprintf("FrameBuffer Reset: width [%d] height: [%d]\n",
	//         width, height);
	DestroyBuffer();
	// // 1. Create the frame buffer
	glGenFramebuffers(NUM_BUFFERS, &mBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, mBufferId);

	// // 2. Create texture to associate w/ frame buffer
	GLuint prevTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) &prevTexture);
	{
		glGenTextures(NUM_BUFFERS, &mTextureId);
	 	glBindTexture(GL_TEXTURE_2D, mTextureId);

		const int LEVEL_OF_DETAIL = 0;
		glTexImage2D(GL_TEXTURE_2D,
					 LEVEL_OF_DETAIL,
					 GL_RGB,
					 width, height,
					 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	// 3. Set texture as framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER,
	                    GL_COLOR_ATTACHMENT0,
						GL_TEXTURE_2D,
	                    mTextureId, 0);

	// 4. Set list of draw buffers (not really sure what this is!)
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		dsprintf("Error: Failed to setup framebuffer.\n");
	}

	// Restore previous texture
	glBindTexture(GL_TEXTURE_2D, prevTexture);
	Disable();
}

void FrameBuffer::Enable()
{
	assert(mBufferId != 0);
	glBindFramebuffer(GL_FRAMEBUFFER, mBufferId);
}

void FrameBuffer::Disable()
{
	// Default the draw surface to the window
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

