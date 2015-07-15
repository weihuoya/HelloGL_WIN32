
#include "stdafx.h"


typedef struct _GLContext_
{
	HWND hWnd;
	HDC hDC;
	HGLRC hGLRC;
	HPALETTE hPalette;
} GLContext;


void CheckWindowsError()
{
	DWORD error = ::GetLastError();
	if (error != ERROR_SUCCESS)
	{
		LPSTR message = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
		MessageBoxA(NULL, message, "Windows", MB_ICONERROR | MB_OK);
		LocalFree(message);
		exit(1);
	}
}


void CheckOpenglError()
{
	GLenum error = ::glGetError();
	if (error != GL_NO_ERROR)
	{
		MessageBoxA(NULL, (const char *)gluErrorString(error), "OpenGL", MB_ICONERROR | MB_OK);
		exit(1);
	}
}



BOOL GLContext_SetPixelFormat(GLContext * context)
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  /* size */
		1,                              /* version */
		PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER, /* support double-buffering */
		PFD_TYPE_RGBA,                  /* color type */
		16,                             /* prefered color depth */
		0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
		0,                              /* no alpha buffer */
		0,                              /* alpha bits (ignored) */
		0,                              /* no accumulation buffer */
		0, 0, 0, 0,                     /* accum bits (ignored) */
		16,                             /* depth buffer */
		0,                              /* no stencil buffer */
		0,                              /* no auxiliary buffers */
		PFD_MAIN_PLANE,                 /* main layer */
		0,                              /* reserved */
		0, 0, 0,                        /* no layer, visible, damage masks */
	};

	int pixelFormat = ChoosePixelFormat(context->hDC, &pfd);
	if (pixelFormat > 0 && SetPixelFormat(context->hDC, pixelFormat, &pfd))
	{
		return TRUE;
	}

	return FALSE;
}


BOOL GLContext_CreatePalette(GLContext * context)
{
	PIXELFORMATDESCRIPTOR pfd;
	int pixelFormat = GetPixelFormat(context->hDC);
	DescribePixelFormat(context->hDC, pixelFormat, sizeof(pfd), &pfd);
	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		WORD i, paletteSize = 1 << pfd.cColorBits;
		DWORD memSize = sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY);
		DWORD redMask = (1 << pfd.cRedBits) - 1;
		DWORD greenMask = (1 << pfd.cGreenBits) - 1;
		DWORD blueMask = (1 << pfd.cBlueBits) - 1;

		LOGPALETTE * pPalette = (LOGPALETTE*)malloc(memSize);
		memset(pPalette, 0, memSize);

		pPalette->palVersion = 0x300;
		pPalette->palNumEntries = paletteSize;

		for (i = 0; i < paletteSize; ++i)
		{
			pPalette->palPalEntry[i].peRed = (((i >> pfd.cRedShift)   & redMask) * 255) / redMask;
			pPalette->palPalEntry[i].peGreen = (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
			pPalette->palPalEntry[i].peBlue = (((i >> pfd.cBlueShift)  & blueMask) * 255) / blueMask;
			pPalette->palPalEntry[i].peFlags = 0;
		}

		context->hPalette = CreatePalette(pPalette);
		free(pPalette);

		if (context->hPalette)
		{
			SelectPalette(context->hDC, context->hPalette, FALSE);
			RealizePalette(context->hDC);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL GLContext_CreateGLRC(GLContext * context)
{
	context->hGLRC = wglCreateContext(context->hDC);
	wglMakeCurrent(context->hDC, context->hGLRC);

	/* set viewing projection */
	glMatrixMode(GL_PROJECTION);
	glFrustum(-0.5F, 0.5F, -0.5F, 0.5F, 1.0F, 3.0F);

	/* position viewer */
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.0F, 0.0F, -2.0F);

	/* position object */
	glRotatef(30.0F, 1.0F, 0.0F, 0.0F);
	glRotatef(30.0F, 0.0F, 1.0F, 0.0F);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// anti-alias
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	CheckOpenglError();

	return TRUE;
}


void GLContext_DrawFrame(GLContext * context)
{
	/* clear color and depth buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw six faces of a cube */
	glBegin(GL_QUADS);
	glNormal3f(0.0F, 0.0F, 1.0F);
	glVertex3f(0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);
	glVertex3f(-0.5F, -0.5F, 0.5F); glVertex3f(0.5F, -0.5F, 0.5F);

	glNormal3f(0.0F, 0.0F, -1.0F);
	glVertex3f(-0.5F, -0.5F, -0.5F); glVertex3f(-0.5F, 0.5F, -0.5F);
	glVertex3f(0.5F, 0.5F, -0.5F); glVertex3f(0.5F, -0.5F, -0.5F);

	glNormal3f(0.0F, 1.0F, 0.0F);
	glVertex3f(0.5F, 0.5F, 0.5F); glVertex3f(0.5F, 0.5F, -0.5F);
	glVertex3f(-0.5F, 0.5F, -0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);

	glNormal3f(0.0F, -1.0F, 0.0F);
	glVertex3f(-0.5F, -0.5F, -0.5F); glVertex3f(0.5F, -0.5F, -0.5F);
	glVertex3f(0.5F, -0.5F, 0.5F); glVertex3f(-0.5F, -0.5F, 0.5F);

	glNormal3f(1.0F, 0.0F, 0.0F);
	glVertex3f(0.5F, 0.5F, 0.5F); glVertex3f(0.5F, -0.5F, 0.5F);
	glVertex3f(0.5F, -0.5F, -0.5F); glVertex3f(0.5F, 0.5F, -0.5F);

	glNormal3f(-1.0F, 0.0F, 0.0F);
	glVertex3f(-0.5F, -0.5F, -0.5F); glVertex3f(-0.5F, -0.5F, 0.5F);
	glVertex3f(-0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F, -0.5F);
	glEnd();

	SwapBuffers(context->hDC);
}


void GLContext_Resize(GLContext * context, int nWidth, int nHeight)
{
	/* set viewport to cover the window */
	glViewport(0, 0, nWidth, nHeight);

	GLContext_DrawFrame(context);
}


void GLContext_UpdatePalette(GLContext * context)
{
	UnrealizeObject(context->hPalette);
	SelectPalette(context->hDC, context->hPalette, FALSE);
	RealizePalette(context->hDC);

	GLContext_DrawFrame(context);
}


void GLContext_Destroy(GLContext * context)
{
	if (context->hGLRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(context->hGLRC);
	}

	if (context->hPalette)
	{
		DeleteObject(context->hPalette);
	}

	if (context->hDC)
	{
		ReleaseDC(context->hWnd, context->hDC);
	}

	free(context);
}


LRESULT APIENTRY GLContext_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GLContext * context = (GLContext *)GetWindowLong(hWnd, GWL_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
		context = (GLContext *)reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)context);
		context->hDC = GetDC(hWnd);
		context->hWnd = hWnd;

		GLContext_SetPixelFormat(context);
		GLContext_CreatePalette(context);
		GLContext_CreateGLRC(context);
		return 0;
	case WM_DESTROY:
		GLContext_Destroy(context);
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:		
		GLContext_Resize(context, LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_PALETTECHANGED:
		if ((HWND)wParam != hWnd)
		{
			GLContext_UpdatePalette(context);
		}
		break;
	case WM_QUERYNEWPALETTE:
		GLContext_UpdatePalette(context);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		GLContext_DrawFrame(context);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_CHAR:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
			return 0;
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


GLContext * GLContext_Create()
{
	GLContext * context = (GLContext *)malloc(sizeof(GLContext));
	memset(context, 0, sizeof(GLContext));
	return context;
}


HWND GLContext_CreateWindow(GLContext * context)
{
	int nX = 0, nY = 0, nWidth = 640, nHeight = 480;
	WNDCLASS wndClass;
	LPCSTR lpszWindowName = _T("WGL");
	LPCSTR lpszClassName = _T("WGL");
	HINSTANCE hInstance = GetModuleHandle(NULL);

	if (!GetClassInfo(hInstance, lpszClassName, &wndClass))
	{
		/* register window class */
		wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = GLContext_WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = lpszClassName;

		if (!RegisterClass(&wndClass))
		{
			return FALSE;
		}
	}

	/* create window */
	return CreateWindow(
		lpszClassName,
		lpszWindowName,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		nX, nY, nWidth, nHeight,
		NULL, NULL, hInstance, context);
}


DWORD GLContext_Run(GLContext * context)
{
	MSG msg;

	/* display window */
	ShowWindow(context->hWnd, SW_SHOWDEFAULT);
	UpdateWindow(context->hWnd);

	/* process messages */
	while (GetMessage(&msg, NULL, 0, 0) == TRUE)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


/*int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	GLContext * context = GLContext_Create();

	if (GLContext_CreateWindow(context))
	{
		return GLContext_Run(context);
	}
	else
	{
		GLContext_Destroy(context);
	}

	return -1;
}*/


int _tmain(int argc, TCHAR * argv[])
{
	GLContext * context = GLContext_Create();

	if (GLContext_CreateWindow(context))
	{
		return GLContext_Run(context);
	}
	else
	{
		GLContext_Destroy(context);
	}

	return -1;
}