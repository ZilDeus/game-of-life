#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_
#include "../include/shader.h"
struct Framebuffer
{
	unsigned int fboObject,rboObject;
	int width,height;
	unsigned int _lastActiveIndex;
	void* textures;
};
struct Framebuffer* Framebuffer(int width,int height);

struct Framebuffer* FramebufferUpdateSize(struct Framebuffer* fb,int width,int height);

void FramebufferAddTextures(struct Framebuffer* fb,unsigned int count);

void FramebufferRender(struct Framebuffer* fb,struct Shader* shader);

void* FramebufferGetTexture(struct Framebuffer* fb,int index);

void FramebufferActivate(struct Framebuffer* fb);

void FramebufferClear(struct Framebuffer* fb);

void FramebufferFree(struct Framebuffer* fb);
#endif
