#include <windows.h>
#include <GL/gl.h>
#include "glext.h"

#ifdef __cplusplus
extern "C" 
{
#endif
int  _fltused = 0;
#ifdef __cplusplus
}
#endif

#define XRES 800
#define YRES 600

static const char *g_shader =
"#version 330\n"
"uniform float t;"
"out vec4 o;"
"void main()"
"{"
	"vec2 u=(gl_FragCoord.xy/600);"
	"u-=vec2((800/600)/2.,.5);"	
	"vec3 g=vec3(0);"
	"for(float i=1.;i<100;i++){"
		"float q=t*.05-.6*i;"
		"vec2 p=vec2(.75*sin(q),.5*sin(q));"
		"p+=vec2(.75*cos(q*0.1),.5*sin(q*.3));"
		"p/=4.*sin(i);"
		"float m=.005/((u.x-p.x)*(u.x-p.x)+(u.y-p.y)*(u.y-p.y))/i;"
		"g+=vec3(m/2.,m/3.,m);"
	"}"
	"vec3 c=vec3(0,0,0);"
	"c+=pow(g,vec3(1.1,.1,0));"
    "o=vec4(c,1);"
"}";

static PIXELFORMATDESCRIPTOR pfd = {
    0, 1, PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 32, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0
};

#ifdef _DEBUG
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    HDC hDC = GetDC(CreateWindow("edit", 0, WS_POPUP|WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0));
#else

static DEVMODE dmScreenSettings = {
    "", 0, 0, sizeof(dmScreenSettings), 0, DM_PELSWIDTH|DM_PELSHEIGHT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0, XRES, YRES, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

__declspec(naked) void winmain() {
  	__asm enter 0x10, 0;
	__asm pushad;

    {

    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
    HDC hDC = GetDC(CreateWindow("edit", 0, WS_POPUP|WS_VISIBLE|WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0));

#endif
    SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
    wglMakeCurrent(hDC, wglCreateContext(hDC));

#ifndef _DEBUG
    ShowCursor(FALSE);
#endif

    GLuint prog = ((PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"))();
    GLuint shader = ((PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader"))(GL_FRAGMENT_SHADER);
    ((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(shader, 1, &g_shader, 0);
    ((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(shader);
    ((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(prog, shader);
    ((PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram"))(prog);
    ((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(prog);

#ifdef _DEBUG

    PFNGLGETSHADERIVPROC glGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");

    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog");
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");

    char str[512] = {0};
    glGetProgramInfoLog(prog, sizeof(str), NULL, str);
    OutputDebugString("---- Program info and shader info log ----\n");
    OutputDebugString(str);
    memset(str, 0, 512);
    glGetShaderInfoLog(shader, sizeof(str), NULL, str);
    OutputDebugString(str);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        OutputDebugString("Failed to compile shader. Aborting!");
	    return -1;
    }
    OutputDebugString("------------------------------------------\n");

#endif
    do {
        const float time = timeGetTime() / 100.0f;
        ((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(prog, "t"), time);
        glRects(-1, -1, 1, 1);
        SwapBuffers(hDC);
        PeekMessageA(0, 0, 0, 0, PM_REMOVE);
    } while (!GetAsyncKeyState(VK_ESCAPE));

#ifndef _DEBUG
	}
#endif

    ExitProcess(0);
}
