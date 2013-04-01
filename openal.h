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

#endif // OPENAL_H
