#include "audiointerface.h"

void AudioInterface::suspend(bool v)
{
    if (mSuspended != v) {
        mSuspended = v;
        suspendChanged(v);
    }
}

void AudioInterface::resume(bool v)
{
    suspend(!v);
}
