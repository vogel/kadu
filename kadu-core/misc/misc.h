/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KADU_MISC_H
#define KADU_MISC_H

#include <QtGui/QApplication>

#include "coding-conversion.h"
#include "date-time.h"
#include "path-conversion.h"

// private
extern KADUAPI long int startTime, beforeExecTime, endingTime, exitingTime;
extern KADUAPI bool measureTime;

KADUAPI QString pwHash(const QString &text);
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);

void KADUAPI saveWindowGeometry(const QWidget *w, const QString &section, const QString &name);
void KADUAPI loadWindowGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

QRect stringToRect(const QString &value, const QRect *def = NULL);
QString rectToString(const QRect& rect);

QList<int> stringToIntList(const QString &in);
QString intListToString(const QList<int> &in);

/**
	zast�pstwo dla arga w QString, kt�re podmienia kolejne %[1-4] w miejscu

	w QStringu efektem:
		QString("%1 odst�p %2").arg("pierwszy %1 tekst").arg("drugi tekst") jest "pierwszy drugi tekst tekst odst�p %2"
	a chcieliby�my �eby by�o
		"pierwszy %1 tekst odst�p drugi tekst"
	co robi w�a�nie ta funkcja
**/
KADUAPI QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3=QString(), const QString &arg4=QString());

KADUAPI QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3, const QString &arg4,
				const QString &arg5, const QString &arg6=QString(),
				const QString &arg7=QString(),const QString &arg8=QString(),
				const QString &arg9=QString());

/**
	uog�lniony narg(const QString&, const QString &, const QString &, const QString &, const QString &)
	na wi�ksz� liczb� parametr�w
	count musi by� <=9
	tab - tablica count wska�nik�w do QString
**/
KADUAPI QString narg(const QString &s, const QString **tab, int count);

QString fixFileName(const QString &path, const QString &fn);

#endif // MISC_H
