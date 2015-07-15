#ifndef MAINDEF_H
#define MAINDEF_H
#pragma once
#define GLAPI __declspec(dllexport)
#include <Windows.h>

void __glext_loadproc(const char * func_name, void** funcptr);

/* GLAPI void APIENTRY function(args...); */
#define GLPROC_VOID(procname, args, argvar) \
extern "C" GLAPI void APIENTRY procname args \
{ \
	static void (APIENTRY *__func) args = NULL; \
    __glext_loadproc(#procname, (void**) &__func); \
	if(__func) \
						{ \
	  __func argvar ;\
						} \
}

#define GLPROC_RETURN(rettype, procname, args, argvar, failAC) \
 extern "C" GLAPI rettype APIENTRY procname args \
{ \
	static rettype (APIENTRY *__func) args = NULL; \
	__glext_loadproc(#procname, (void**)&__func); \
	if(__func) \
					{ \
	  return __func argvar ;\
					} \
																else  \
	{\
		failAC\
	}\
}

#define GLPROC_RETURN1(rettype, procname, args, argvar)  GLPROC_RETURN(rettype, procname, args, argvar, { return (rettype) 0; })


#define GLPROCEX_VOID(procname, _type, procstr, args, argvar) \
 extern "C" GLAPI  void APIENTRY procname args \
{ \
	static void (_type *__func) args = NULL; \
	__glext_loadproc(procstr, (void**)&__func); \
	if(__func) \
				{ \
	  __func argvar ;\
				} \
}

#define GLPROCEX_RETURN(rettype, _type, procname, procstr, args, argvar, failAC) \
 extern "C" GLAPI rettype APIENTRY procname args \
{ \
	static rettype (_type *__func) args = NULL; \
	__glext_loadproc(procstr, (void**)&__func); \
	if(__func) \
			{ \
	  return __func argvar ;\
			} \
				else  \
	{\
		failAC\
	}\
}


// WGL
#define WGLPROC_VOID(procname, args, argvar) \
extern "C" GLAPI void WINAPI procname args \
{ \
	static void (APIENTRY *__func) args = NULL; \
	__glext_loadproc(#procname, (void**)&__func); \
	if(__func) \
		{ \
	  __func argvar ;\
		} \
}

#define WGLPROC_RETURN(rettype, procname, args, argvar, failAC) \
 extern "C" GLAPI rettype WINAPI procname args \
{ \
	static rettype (APIENTRY *__func) args = NULL; \
	__glext_loadproc(#procname, (void**)&__func); \
	if(__func) \
	{ \
	  return __func argvar ;\
	} \
	else  \
	{\
		failAC\
	}\
}

#define WGLPROC_RETURN1(rettype, procname, args, argvar) WGLPROC_RETURN(rettype, procname, args, argvar, { return (rettype) 0; })

// Some other helper
extern "C" GLAPI void* APIENTRY glExtGetProcAddress(const char* func_name);

#endif