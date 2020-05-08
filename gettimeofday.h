#ifndef LIBMPEG2_GETTIMEOFDAY_H
#define LIBMPEG2_GETTIMEOFDAY_H

#if defined(HAVE_STRUCT_TIMEVAL) && defined(HAVE_GETTIMEOFDAY)
#if defined(TIME_WITH_SYS_TIME)
#include <sys/time.h>
#include <time.h>
#elif defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#else
#include <time.h>
#endif
#elif defined(HAVE_SYS_TIMEB_H) && defined(HAVE_FTIME)

#define HAVE_GETTIMEOFDAY 1
#define CUSTOM_GETTIMEOFDAY 1

struct timeval {
    long tv_sec;
    long tv_usec;
};

void gettimeofday (struct timeval * tp, void * dummy);

#else
#undef HAVE_GETTIMEOFDAY
#endif

#endif /* LIBMPEG2_GETTIMEOFDAY_H */
