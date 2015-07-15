#include "loader.h"
#include <gl/GL.h>

/* module opengl32.dll must be loaded */
static HMODULE hOpenGL = 0;

void __glext_loadproc(const char* func_name, void** funcptr)
{
	/* try finding the procedure via wglGetProcAddress */
	PROC func = wglGetProcAddress(func_name);

	if (!func)
	{
		if (!hOpenGL)
		{
			hOpenGL = LoadLibraryA("opengl32");
			if (!hOpenGL)
			{
				hOpenGL = LoadLibraryA("opengl32.dll");
			}
		}

		if (hOpenGL)
		{
			/* try loading statically from opengl32.dll */
			func = (PROC)GetProcAddress(hOpenGL, func_name);
		}
	}

	*funcptr = func;
}

extern "C" GLAPI void* APIENTRY glExtGetProcAddress(const char* func_name)
{
	void * __func = NULL;
	__glext_loadproc(func_name, &__func);
	return __func;
}