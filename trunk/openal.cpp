#include "openal.h"
#include <QDebug>

#ifdef WIN32
HMODULE openAlLib = 0;
#endif

LPALGETERROR alGetError_;
LPALDELETESOURCES alDeleteSources_;
LPALDELETEBUFFERS alDeleteBuffers_;
LPALGENSOURCES alGenSources_;
LPALGENBUFFERS alGenBuffers_;
LPALBUFFERDATA alBufferData_;
LPALSOURCEI alSourcei_;
LPALSOURCEPLAY alSourcePlay_;
LPALSOURCESTOP alSourceStop_;

LPALCGETSTRING alcGetString_;
LPALCOPENDEVICE alcOpenDevice_;
LPALCCREATECONTEXT alcCreateContext_;
LPALCMAKECONTEXTCURRENT alcMakeContextCurrent_;
LPALCGETCURRENTCONTEXT alcGetCurrentContext_;
LPALCGETCONTEXTSDEVICE alcGetContextsDevice_;
LPALCDESTROYCONTEXT alcDestroyContext_;
LPALCCLOSEDEVICE alcCloseDevice_;

bool loadOpenAL()
{
#ifdef WIN32
    qDebug() << "Loading OPENAL32.DLL";
    openAlLib = LoadLibraryW(L"openal32.dll");
    if (openAlLib == 0)
        return false;

    alGetError_ = (LPALGETERROR) GetProcAddress(openAlLib,"alGetError");
    alDeleteBuffers_ = (LPALDELETEBUFFERS) GetProcAddress(openAlLib,"alDeleteBuffers");
    alDeleteSources_ = (LPALDELETESOURCES) GetProcAddress(openAlLib,"alDeleteSources");
    alGenSources_ = (LPALGENSOURCES) GetProcAddress(openAlLib,"alGenSources");
    alGenBuffers_ = (LPALGENBUFFERS) GetProcAddress(openAlLib,"alGenBuffers");
    alBufferData_ = (LPALBUFFERDATA) GetProcAddress(openAlLib,"alBufferData");
    alSourcei_ = (LPALSOURCEI) GetProcAddress(openAlLib,"alSourcei");
    alSourcePlay_ = (LPALSOURCEPLAY) GetProcAddress(openAlLib,"alSourcePlay");
    alSourceStop_ = (LPALSOURCESTOP) GetProcAddress(openAlLib,"alSourceStop");

    alcGetString_ = (LPALCGETSTRING) GetProcAddress(openAlLib,"alcGetString");
    alcOpenDevice_ = (LPALCOPENDEVICE) GetProcAddress(openAlLib,"alcOpenDevice");
    alcCreateContext_ = (LPALCCREATECONTEXT) GetProcAddress(openAlLib,"alcCreateContext");
    alcMakeContextCurrent_ = (LPALCMAKECONTEXTCURRENT) GetProcAddress(openAlLib,"alcMakeContextCurrent");
    alcGetCurrentContext_ = (LPALCGETCURRENTCONTEXT) GetProcAddress(openAlLib,"alcGetCurrentContext");
    alcGetContextsDevice_ = (LPALCGETCONTEXTSDEVICE) GetProcAddress(openAlLib,"alcGetContextsDevice");
    alcDestroyContext_ = (LPALCDESTROYCONTEXT) GetProcAddress(openAlLib,"alcDestroyContext");
    alcCloseDevice_ = (LPALCCLOSEDEVICE) GetProcAddress(openAlLib,"alcCloseDevice");

    if (alGetError_ == NULL) { qDebug() << "alGetError is NULL"; return false; }
    if (alDeleteBuffers_ == NULL) { qDebug() << "alDeleteBuffers is NULL"; return false; }
    if (alDeleteSources_ == NULL) { qDebug() << "alDeleteSources is NULL"; return false; }
    if (alGenSources_ == NULL) { qDebug() << "alGenSources is NULL"; return false; }
    if (alGenBuffers_ == NULL) { qDebug() << "alGenBuffers is NULL"; return false; }
    if (alBufferData_ == NULL) { qDebug() << "alBufferData is NULL"; return false; }
    if (alSourcei_ == NULL) { qDebug() << "alSourcei is NULL"; return false; }
    if (alSourcePlay_ == NULL) { qDebug() << "alSourcePlay is NULL"; return false; }
    if (alSourceStop_ == NULL) { qDebug() << "alSourceStop is NULL"; return false; }
    if (alcGetString_ == NULL) { qDebug() << "alcGetString is NULL"; return false; }
    if (alcOpenDevice_ == NULL) { qDebug() << "alcOpenDevice is NULL"; return false; }
    if (alcCreateContext_ == NULL) { qDebug() << "alcCreateContext is NULL"; return false; }
    if (alcMakeContextCurrent_ == NULL) { qDebug() << "alcMakeContextCurrent is NULL"; return false; }
    if (alcGetCurrentContext_ == NULL) { qDebug() << "alcGetCurrentContext is NULL"; return false; }
    if (alcGetContextsDevice_ == NULL) { qDebug() << "alcGetContextsDevice is NULL"; return false; }
    if (alcDestroyContext_ == NULL) { qDebug() << "alcDestroyContext is NULL"; return false; }
    if (alcCloseDevice_ == NULL) { qDebug() << "alcCloseDevice is NULL"; return false; }
#else // WIN32
    alGetError_ = alGetError;
    alDeleteBuffers_ = alDeleteBuffers;
    alDeleteSources_ = alDeleteSources;
    alGenSources_ = alGenSources;
    alGenBuffers_ = alGenBuffers;
    alBufferData_ = alBufferData;
    alSourcei_ = alSourcei;
    alSourcePlay_ = alSourcePlay;
    alSourceStop_ = alSourceStop;
    alcGetString_ = alcGetString;
    alcOpenDevice_ = alcOpenDevice;
    alcCreateContext_ = alcCreateContext;
    alcMakeContextCurrent_ = alcMakeContextCurrent;
    alcGetCurrentContext_ = alcGetCurrentContext;
    alcGetContextsDevice_ = alcGetContextsDevice;
    alcDestroyContext_ = alcDestroyContext;
    alcCloseDevice_ = alcCloseDevice;
#endif // WIN32

    return true;
}

void unloadOpenAL()
{
#ifdef WIN32
    qDebug() << "Unloading OPENAL32.DLL";
    if (openAlLib != 0)
        FreeLibrary(openAlLib);

    openAlLib = 0;
#endif // WIN32
}
