#include "debug.h"

#ifdef DEBUG_ENABLED

#include <qmutex.h>
#include <stdio.h>
#include <stdarg.h>

/*
	Poniewa¿ debug() mo¿e byæ u¿ywany w ró¿nych w±tkach,
	wiêc zastosowa³em semafor, aby unikn±æ wy¶wietlenia
	na ekranie czego¶ przez inny w±tek pomiêdzy
	poszczególnymi wywo³aniami fprintf
*/
static QMutex debug_mutex;
int debug_mask;

void _kdebug_with_mask(int mask, const char* file,const int line,const char* format,...)
{
	if (debug_mask & mask)
	{
		debug_mutex.lock();
		fprintf(stderr,"KK <%s:%i>\t",file,line);
		va_list args;
		va_start(args,format);
		vfprintf(stderr,format,args);
		va_end(args);
		debug_mutex.unlock();
	}
}

#endif
