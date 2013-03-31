#ifndef OPENAL_H
#define OPENAL_H

#ifdef WIN32
#include <windows.h>
#include "AL/al.h"
#include "AL/alc.h"
#else // WIN32
#include <AL/al.h>
#include <AL/alc.h>
#endif // WIN32

#ifdef WIN32
extern HMODULE openAlLib;
#endif

bool loadOpenAL();
void unloadOpenAL();

extern LPALGETERROR alGetError_;
extern LPALDELETESOURCES alDeleteSources_;
extern LPALDELETEBUFFERS alDeleteBuffers_;
extern LPALGENSOURCES alGenSources_;
extern LPALGENBUFFERS alGenBuffers_;
extern LPALBUFFERDATA alBufferData_;
extern LPALSOURCEI alSourcei_;
extern LPALSOURCEPLAY alSourcePlay_;
extern LPALSOURCESTOP alSourceStop_;

extern LPALCGETSTRING alcGetString_;
extern LPALCOPENDEVICE alcOpenDevice_;
extern LPALCCREATECONTEXT alcCreateContext_;
extern LPALCMAKECONTEXTCURRENT alcMakeContextCurrent_;
extern LPALCGETCURRENTCONTEXT alcGetCurrentContext_;
extern LPALCGETCONTEXTSDEVICE alcGetContextsDevice_;
extern LPALCDESTROYCONTEXT alcDestroyContext_;
extern LPALCCLOSEDEVICE alcCloseDevice_;

#endif // OPENAL_H
