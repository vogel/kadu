#ifndef DEBUG_H
#define DEBUG_H

#include "kadu-config.h"

#define KADU_DEBUG_FUNCTION_START (1<<8)
#define KADU_DEBUG_FUNCTION_END   (1<<9)

#define KADU_DEBUG_INFO           (1<<10)
#define KADU_DEBUG_WARNING        (1<<11)
#define KADU_DEBUG_ERROR          (1<<12)
#define KADU_DEBUG_PANIC          (1<<13)

#define KADU_DEBUG_DUMP           (1<<14)
#define KADU_DEBUG_NETWORK        (1<<15)

#define KADU_DEBUG_ALL 2147483647


/*
	<<< kdebug >>>
	Wy¶wietla komunikat debuguj±cy na konsoli.
	Sk³adnia jak w printf.
*/
#ifdef DEBUG_ENABLED
#define kdebug(format,args...) \
	_kdebug_with_mask(KADU_DEBUG_ALL,__FILE__,__LINE__,format,##args)
#define kdebugm(mask,format,args...) \
	_kdebug_with_mask(mask,__FILE__,__LINE__,format,##args)
#else
#define kdebug(format,args...)
#define kdebugm(mask,format,args...)
#endif


/*
	<<< kdebugf >>>
	Wy¶wietla komunikat debuguj±cy zawieraj±cy
	nazwê aktualnie wykonywanej funkcji.
	Z za³o¿enia makro to powinno byæ wywo³ane
	w pierwszej linii ka¿dej funkcji. Dziêki
	temu mo¿na bêdzie w przysz³o¶ci ¶ledziæ
	dzia³anie programu.
*/	
#define kdebugf() \
	kdebugm(KADU_DEBUG_FUNCTION_START, "%s\n",__PRETTY_FUNCTION__)

#define kdebugf2() \
	kdebugm(KADU_DEBUG_FUNCTION_END, "%s end\n",__PRETTY_FUNCTION__)

/*
	Funkcja pomocnicza. Nie u¿ywaæ.
*/
#ifdef DEBUG_ENABLED
void _kdebug_with_mask(int mask, const char* file,const int line,const char* format,...);
#endif
extern int debug_mask;

#endif
