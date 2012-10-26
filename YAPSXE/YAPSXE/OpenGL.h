#pragma once
#ifndef OPENGL_H
#define OPENGL_H

#include "Common.h"

extern PFNGLISRENDERBUFFEREXTPROC						glIsRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC						glBindRenderbufferEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC					glDeleteRenderbuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC						glGenRenderbuffersEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC					glRenderbufferStorageEXT;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			glGetRenderbufferParameterivEXT;
extern PFNGLISFRAMEBUFFEREXTPROC						glIsFramebufferEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC						glBindFramebufferEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC					glDeleteFramebuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC						glGenFramebuffersEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				glCheckFramebufferStatusEXT;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC					glFramebufferTexture1DEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC					glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC					glFramebufferTexture3DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				glFramebufferRenderbufferEXT;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	glGetFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATEMIPMAPEXTPROC						glGenerateMipmapEXT;
extern PFNGLDRAWBUFFERSPROC								glDrawBuffers;

class CGLRenderer {
public:
	CGLRenderer();
	~CGLRenderer();

	BOOL InitOpenGLWindow(CWindow *wnd);
	void DestroyOpenGL();

	BOOL CreateFrameBufferObject(int width, int height);
	void RenderFrameBufferObject();

	void ToggleFullscreenMode();

	HGLRC hRC;

	u32 mFboID;
	u32 mDepthBufferID;
	u32 mRenderTextureID;
	int mFboWidth;
	int mFboHeight;
	int mFboRenderWidth;
	int mFboRenderHeight;
	int mMaxTextureSize;

	BOOL mInitialised;

//private:
	CWindow *glWindow;
	Console *csl;
};

#endif /* OPENGL_H */