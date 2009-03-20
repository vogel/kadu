#ifndef PATH_CONVERSION_H
#define PATH_CONVERSION_H

#include "base.h"

KADUAPI void printBacktrace(const QString &header = QString::null);

/**
	Zmienia ścieżkę relatywną do katalogu z ustawieniami gg
	na ścieżkę bezwzgledną uwzględniając zmienne środowiskowe
	$HOME i $CONFIG_DIR
**/
KADUAPI QString ggPath(const QString &subpath = QString::null);

/**
	zwraca ścieżkę do pliku f
	jeżeli drugi parametr nie jest == 0, to funkcja próbuje najpierw ustalić
	ścieżkę na podstawie argv0, które ma być równe argv[0] oraz zmiennej PATH
**/
KADUAPI QString dataPath(const QString &f = QString::null, const char *argv0 = 0);

KADUAPI QString libPath(const QString &f = QString::null);

#endif
