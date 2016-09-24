#ifndef WINRTBRIDGE_H
#define WINRTBRIDGE_H

#ifdef WINRTBRIDGE_BUILD
# define WINRTBRIDGE_EXTERN __declspec(dllexport)
#else
# define WINRTBRIDGE_EXTERN __declspec(dllimport)
# endif

#include <string>

class WINRTBRIDGE_EXTERN WinRTBridge
{
public:
    WinRTBridge();

    unsigned long long getInstalledPhysicalMemoryInB() const;
};

#endif // WINRTBRIDGE_H
