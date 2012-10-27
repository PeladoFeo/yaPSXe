#include "OpenGL.h"
#include "Windows.h"
#include "Console.h"
#include "Psx.h"
#include "Gpu.h"
#include "Cpu.h"

PFNGLISRENDERBUFFEREXTPROC						glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC					glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC					glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC					glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC					glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC						glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC						glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC					glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC						glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC				glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC				glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC				glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC						glGenerateMipmapEXT = NULL;
PFNGLDRAWBUFFERSPROC							glDrawBuffers = NULL;

#define GL_LOADFN(name)  \
	if( (*(void**)&name = (void*)wglGetProcAddress(#name)) == NULL ) { \
		static char str[256]; \
		sprintf(str, "Failed to load OpenGL extension '%s'", #name); \
		MessageBox(NULL, str, "ERROR", MB_ICONERROR);\
		CPsx::GetInstance()->SignalQuit();\
	}

CGLRenderer::CGLRenderer() {
	csl = CPsx::GetInstance()->csl;

	// must be called before using glew functions
	glewInit();

	glWindow = 0;
	mFboID = 0;
	mDepthBufferID = 0;
	mInitialised = FALSE;
}

CGLRenderer::~CGLRenderer() {
	DestroyOpenGL();
}

void CGLRenderer::DestroyOpenGL() {
	wglMakeCurrent(NULL,NULL);			
	wglDeleteContext(hRC);		
	if (glWindow) {
		ReleaseDC(glWindow->GetHwnd(), glWindow->hDC);
	}
	if (glDeleteFramebuffersEXT && glDeleteRenderbuffersEXT) {
		glDeleteFramebuffersEXT(1, &mFboID);
		glDeleteRenderbuffersEXT(1, &mDepthBufferID);
	}
}

BOOL CGLRenderer::InitOpenGLWindow(CWindow *wnd) {
	glWindow = wnd;
	u32	pixelFormat;	

	static	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),			
		5,									
		PFD_DRAW_TO_WINDOW |					
		PFD_SUPPORT_OPENGL |					
		PFD_DOUBLEBUFFER,						
		PFD_TYPE_RGBA,							
		32,									
		0, 0, 0, 0, 0, 0,						
		0,										
		0,										
		0,										
		0, 0, 0, 0,								
		56,										
		0,											
		0,										
		PFD_MAIN_PLANE,							
		0,											
		0, 0, 0									
	};
	
	if (!(wnd->hDC=GetDC(wnd->GetHwnd()))) {
		return FALSE;								
	}

	if (!(pixelFormat=ChoosePixelFormat(wnd->hDC,&pfd))) {
		return FALSE;							
	}

	if(!SetPixelFormat(wnd->hDC,pixelFormat,&pfd)) {
		return FALSE;							
	}

	if (!(hRC=wglCreateContext(wnd->hDC))) {
		return FALSE;							
	}

	if(!wglMakeCurrent(glWindow->hDC,hRC)) {
		return FALSE;							
	}		

	glViewport(0,0,glWindow->width,glWindow->height);					

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 		

	static BOOL bExtensionsLoaded = FALSE;
	if (!bExtensionsLoaded) {
		GL_LOADFN(glIsRenderbufferEXT);
		GL_LOADFN(glBindRenderbufferEXT);
		GL_LOADFN(glDeleteRenderbuffersEXT);
		GL_LOADFN(glGenRenderbuffersEXT);
		GL_LOADFN(glRenderbufferStorageEXT);
		GL_LOADFN(glGetRenderbufferParameterivEXT);
		GL_LOADFN(glIsFramebufferEXT);
		GL_LOADFN(glBindFramebufferEXT);
		GL_LOADFN(glDeleteFramebuffersEXT);
		GL_LOADFN(glGenFramebuffersEXT);
		GL_LOADFN(glCheckFramebufferStatusEXT);
		GL_LOADFN(glFramebufferTexture1DEXT);
		GL_LOADFN(glFramebufferTexture2DEXT);
		GL_LOADFN(glFramebufferTexture3DEXT);
		GL_LOADFN(glFramebufferRenderbufferEXT);
		GL_LOADFN(glGetFramebufferAttachmentParameterivEXT);
		GL_LOADFN(glGenerateMipmapEXT);
		bExtensionsLoaded = TRUE;
	}

	if (!CreateFrameBufferObject(CGpu::VRAM_WIDTH, CGpu::VRAM_HEIGHT)) {
		MessageBox(CPsx::GetInstance()->mMainWnd->GetHwnd(), 
			"OpenGL FBO extensions unsupported", "Error", MB_ICONERROR);
		return FALSE;
	}

	mFboRenderWidth = CGpu::VRAM_WIDTH;
	mFboRenderHeight = CGpu::VRAM_HEIGHT;

	mInitialised = TRUE;
	return TRUE;
}

BOOL CGLRenderer::CreateFrameBufferObject(int width, int height) {
	mFboWidth = width;
	mFboHeight = height;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mRenderTextureID);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize);

    // RGBA8 2D texture, 24 bit depth texture,
	glGenTextures(1, &mRenderTextureID);
	glBindTexture(GL_TEXTURE_2D, mRenderTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// reserve texture memory (texels are undefined)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffersEXT(1, &mFboID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFboID);

	// Attach 2D texture to this FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mRenderTextureID, 0);

	glGenRenderbuffersEXT(1, &mDepthBufferID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mDepthBufferID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, width, height);
	 
	// Attach depth buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthBufferID);

	// Does the GPU support current FBO configuration?
	switch(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) {
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return TRUE;
		default:
			return FALSE;
	}
}

void CGLRenderer::RenderFrameBufferObject() {
	// select the back buffer for rendering
    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);  

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
    glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, mRenderTextureID);
	
	glViewport(0, 0, glWindow->width, glWindow->height);

    glBegin(GL_QUADS);
        glColor4f(1.0, 1.0, 1.0, 1.0);
		
        glTexCoord2f((float)mFboRenderWidth / mFboWidth, 0);  
		glVertex2f(1.0,1.0);
		
        glTexCoord2f(0,0);  
		glVertex2f(-1.0,1.0);
		
        glTexCoord2f(0.0, (float)mFboRenderHeight / mFboHeight);  
		glVertex2f(-1.0,-1.0);
		
        glTexCoord2f((float)mFboRenderWidth / mFboWidth, 
					 (float)mFboRenderHeight / mFboHeight);
		glVertex2f(1.0,-1.0);
		
    glEnd();

	SwapBuffers(glWindow->hDC);

	// switch back to rendering to textures
	glBindFramebufferEXT(GL_FRAMEBUFFER, mFboID); 
}

static void GetMaxDispRes(int &width, int &height) {
	int maxw = 0, maxh = 0;
	DEVMODE mode;

	for (int i = 0; EnumDisplaySettings(NULL, i, &mode); i++) {
		if (ChangeDisplaySettings(&mode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
			if (mode.dmPelsWidth > maxw) maxw = mode.dmPelsWidth;
			if (mode.dmPelsHeight > maxh) maxh = mode.dmPelsHeight;
	}

	width = maxw; 
	height = maxh; 
}

void CGLRenderer::ToggleFullscreenMode() {
	CPsx *psx = CPsx::GetInstance();

	psx->cpu->SetCpuState(PSX_CPU_HALTED);

	glWindow->ShowWnd(FALSE);

	if (!glWindow->mFullscreen) {
		SetWindowLong(glWindow->GetHwnd(), GWL_EXSTYLE, WS_EX_WINDOWEDGE);
		SetWindowLong(glWindow->GetHwnd(), GWL_STYLE, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		GetMaxDispRes(glWindow->width, glWindow->height);
		SetWindowPos(glWindow->GetHwnd(), 0, 0, 0, glWindow->width, glWindow->height, NULL);

		glWindow->mFullscreen = TRUE;
	} else {
		SetWindowLong(glWindow->GetHwnd(), GWL_EXSTYLE, WS_EX_APPWINDOW);
		SetWindowLong(glWindow->GetHwnd(), GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		glWindow->width = 640;
		glWindow->height = 480;
		SetWindowPos(glWindow->GetHwnd(), 0, 0, 0, glWindow->width, glWindow->height, NULL);

		glWindow->mFullscreen = FALSE;
	}

	glWindow->ShowWnd(TRUE);
	SetForegroundWindow(glWindow->GetHwnd());
	SetFocus(glWindow->GetHwnd());

	psx->cpu->SetCpuState(PSX_CPU_RUNNING);
}