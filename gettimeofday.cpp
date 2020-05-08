#include "timeSettings.h"

#include "gettimeofday.h"

#ifdef CUSTOM_GETTIMEOFDAY

#include <sys/timeb.h>

void gettimeofday (struct timeval * tp, void * dummy)
{
    struct timeb tm;
    ftime (&tm);
    tp->tv_sec = tm.time;
    tp->tv_usec = tm.millitm * 1000;
}

#endif
