#include "debug.h"

#include <qstring.h>
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

void _kdebug(const char* file,const int line,const char* format,...)
{
	debug_mutex.lock();
	fprintf(stderr,"%s:%i:  ",file,line);
	va_list args;
	va_start(args,format);
	vfprintf(stderr,format,args);
	va_end(args);
	debug_mutex.unlock();
};
