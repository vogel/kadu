#ifndef DEBUG_H
#define DEBUG_H

#include "kadu-config.h"

/* pierwsze 8 bitow zarezerwowane jest dla libgadu */

/* start funkcji */
#define KDEBUG_FUNCTION_START (1<<8)
/* koniec funkcji */
#define KDEBUG_FUNCTION_END   (1<<9)

/* zwykle inforamcje */
#define KDEBUG_INFO           (1<<10)
/* niegrozne ostrzezenie */
#define KDEBUG_WARNING        (1<<11)
/* powazny blad, ale sytuacja jest do obejscia */
#define KDEBUG_ERROR          (1<<12)
/* blad fatalny, mozliwe ze program sie wywroci */
#define KDEBUG_PANIC          (1<<13)

/* duzo danych do wypisania */
#define KDEBUG_DUMP           (1<<14)
/* informacja dotyczaca obslugi sieci */
#define KDEBUG_NETWORK        (1<<15)

/* 2^31-1 - wszystkie komunikaty */
#define KDEBUG_ALL 2147483647

/* stare-dluzsze wersje tych stalych */
#define KADU_DEBUG_FUNTION_START KDEBUG_FUNCTION_START
#define KADU_DEBUG_FUNTION_END KDEBUG_FUNCTION_END
#define KADU_DEBUG_INFO KDEBUG_INFO
#define KADU_DEBUG_WARNING KDEBUG_WARNING
#define KADU_DEBUG_ERROR KDEBUG_ERROR
#define KADU_DEBUG_PANIC KDEBUG_PANIC
#define KADU_DEBUG_DUMP KDEBUG_DUMP
#define KADU_DEBUG_NETWORK KDEBUG_NETWORK
#define KADU_DEBUG_ALL KDEBUG_ALL

/*
	<<< kdebug >>>
	Wy¶wietla komunikat debuguj±cy na konsoli.
	Sk³adnia jak w printf.
*/
#ifdef DEBUG_ENABLED
#define kdebug(format,args...) \
	_kdebug_with_mask(KDEBUG_ALL,__FILE__,__LINE__,format,##args)
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
	kdebugm(KDEBUG_FUNCTION_START, "%s\n",__PRETTY_FUNCTION__)

#define kdebugf2() \
	kdebugm(KDEBUG_FUNCTION_END, "%s end\n",__PRETTY_FUNCTION__)

/*
	Funkcja pomocnicza. Nie u¿ywaæ.
*/
#ifdef DEBUG_ENABLED
void _kdebug_with_mask(int mask, const char* file,const int line,const char* format,...);
#endif
extern int debug_mask;

#endif
