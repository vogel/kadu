#ifndef DEBUG_H
#define DEBUG_H

#include "../config.h"

/*
	<<< kdebug >>>
	Wy¶wietla komunikat debuguj±cy na konsoli.
	Sk³adnia jak w printf.
*/
#ifdef DEBUG_ENABLED
#define kdebug(format,args...) \
	_kdebug(__FILE__,__LINE__,format,##args)
#else
#define kdebug(format,args...)
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
	kdebug("%s\n",__PRETTY_FUNCTION__)

/*
	Funkcja pomocnicza. Nie u¿ywaæ.
*/
#ifdef DEBUG_ENABLED
void _kdebug(const char* file,const int line,const char* format,...);
#endif

#endif
