/* -LICENSE-START-
** Copyright (c) 2009 Blackmagic Design
**
** Permission is hereby granted, free of charge, to any person or organization
** obtaining a copy of the software and accompanying documentation covered by
** this license (the "Software") to use, reproduce, display, distribute,
** execute, and transmit the Software, and to prepare derivative works of the
** Software, and to permit third-parties to whom the Software is furnished to
** do so, all subject to the following:
** 
** The copyright notices in the Software and this entire statement, including
** the above license grant, this restriction and the following disclaimer,
** must be included in all copies of the Software, in whole or in part, and
** all derivative works of the Software, unless such copies or derivative
** works are solely in the form of machine-executable object code generated by
** a source language processor.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
** -LICENSE-END-
**/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <ctype.h>

#include "DeckLinkAPI.h"

#define kDeckLinkAPI_Name "libDeckLinkAPI.so"
#define KDeckLinkPreviewAPI_Name "libDeckLinkPreviewAPI.so"

typedef IDeckLinkIterator* (*CreateIteratorFunc)(void);
typedef IDeckLinkAPIInformation* (*CreateAPIInformationFunc)(void);
typedef IDeckLinkGLScreenPreviewHelper* (*CreateOpenGLScreenPreviewHelperFunc)(void);
typedef IDeckLinkVideoConversion* (*CreateVideoConversionInstanceFunc)(void);
typedef IDeckLinkDiscovery* (*CreateDeckLinkDiscoveryInstanceFunc)(void);

static pthread_once_t					gDeckLinkOnceControl = PTHREAD_ONCE_INIT;
static pthread_once_t					gPreviewOnceControl = PTHREAD_ONCE_INIT;

static bool								gLoadedDeckLinkAPI = false;

static CreateIteratorFunc					gCreateIteratorFunc = NULL;
static CreateAPIInformationFunc				gCreateAPIInformationFunc = NULL;
static CreateOpenGLScreenPreviewHelperFunc	gCreateOpenGLPreviewFunc = NULL;
static CreateVideoConversionInstanceFunc	gCreateVideoConversionFunc	= NULL;
static CreateDeckLinkDiscoveryInstanceFunc	gCreateDeckLinkDiscoveryFunc = NULL;

static void	InitDeckLinkAPI (void)
{
	void *libraryHandle;
	
	libraryHandle = dlopen(kDeckLinkAPI_Name, RTLD_NOW|RTLD_GLOBAL);
	if (!libraryHandle)
	{
		fprintf(stderr, "%s\n", dlerror());
		return;
	}
	
	gLoadedDeckLinkAPI = true;
	
	gCreateIteratorFunc = (CreateIteratorFunc)dlsym(libraryHandle, "CreateDeckLinkIteratorInstance_0002");
	if (!gCreateIteratorFunc)
		fprintf(stderr, "%s\n", dlerror());
	gCreateAPIInformationFunc = (CreateAPIInformationFunc)dlsym(libraryHandle, "CreateDeckLinkAPIInformationInstance_0001");
	if (!gCreateAPIInformationFunc)
		fprintf(stderr, "%s\n", dlerror());
	gCreateVideoConversionFunc = (CreateVideoConversionInstanceFunc)dlsym(libraryHandle, "CreateVideoConversionInstance_0001");
	if (!gCreateVideoConversionFunc)
		fprintf(stderr, "%s\n", dlerror());
	gCreateDeckLinkDiscoveryFunc = (CreateDeckLinkDiscoveryInstanceFunc)dlsym(libraryHandle, "CreateDeckLinkDiscoveryInstance_0001");
	if (!gCreateDeckLinkDiscoveryFunc)
		fprintf(stderr, "%s\n", dlerror());
}

static void	InitDeckLinkPreviewAPI (void)
{
	void *libraryHandle;
	
	libraryHandle = dlopen(KDeckLinkPreviewAPI_Name, RTLD_NOW|RTLD_GLOBAL);
	if (!libraryHandle)
	{
		fprintf(stderr, "%s\n", dlerror());
		return;
	}
	gCreateOpenGLPreviewFunc = (CreateOpenGLScreenPreviewHelperFunc)dlsym(libraryHandle, "CreateOpenGLScreenPreviewHelper_0001");
	if (!gCreateOpenGLPreviewFunc)
		fprintf(stderr, "%s\n", dlerror());
}

bool		IsDeckLinkAPIPresent (void)
{
	// If the DeckLink API dynamic library was successfully loaded, return this knowledge to the caller
	return gLoadedDeckLinkAPI;
}

IDeckLinkIterator*		CreateDeckLinkIteratorInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);
	
	if (gCreateIteratorFunc == NULL)
		return NULL;
	return gCreateIteratorFunc();
}

IDeckLinkAPIInformation*	CreateDeckLinkAPIInformationInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);
	
	if (gCreateAPIInformationFunc == NULL)
		return NULL;
	return gCreateAPIInformationFunc();
}

IDeckLinkGLScreenPreviewHelper*		CreateOpenGLScreenPreviewHelper (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);
	pthread_once(&gPreviewOnceControl, InitDeckLinkPreviewAPI);
	
	if (gCreateOpenGLPreviewFunc == NULL)
		return NULL;
	return gCreateOpenGLPreviewFunc();
}

IDeckLinkVideoConversion* CreateVideoConversionInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);
	
	if (gCreateVideoConversionFunc == NULL)
		return NULL;
	return gCreateVideoConversionFunc();
}

IDeckLinkDiscovery* CreateDeckLinkDiscoveryInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);
	
	if (gCreateDeckLinkDiscoveryFunc == NULL)
		return NULL;
	return gCreateDeckLinkDiscoveryFunc();
}
