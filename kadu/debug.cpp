#include "debug.h"

#ifdef DEBUG_MODE

#include <qmutex.h>
#include <stdio.h>
#include <stdarg.h>

/*
	Poniewa� debug() mo�e by� u�ywany w r�nych w�tkach,
	wi�c zastosowa�em semafor, aby unikn�� wy�wietlenia
	na ekranie czego� przez inny w�tek pomi�dzy
	poszczeg�lnymi wywo�aniami fprintf
*/
static QMutex debug_mutex;

void _kdebug(const char* file,const int line,const char* format,...)
{
	debug_mutex.lock();
	fprintf(stderr,"KK <%s:%i>\t",file,line);
	va_list args;
	va_start(args,format);
	vfprintf(stderr,format,args);
	va_end(args);
	debug_mutex.unlock();
};

#endif
