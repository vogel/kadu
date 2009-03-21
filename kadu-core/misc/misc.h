/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_MISC_H
#define KADU_MISC_H

#include "choose-description.h"
#include "coding-conversion.h"
#include "date-time.h"
#include "image-dialog.h"
#include "image-widget.h"
#include "path-conversion.h"
#include "token-dialog.h"

extern KADUAPI QFont *defaultFont;
extern KADUAPI QFontInfo *defaultFontInfo;

extern KADUAPI QTextCodec *codec_cp1250;
extern KADUAPI QTextCodec *codec_latin2;
// private
extern KADUAPI long int startTime, beforeExecTime, endingTime, exitingTime;
extern KADUAPI bool measureTime;

KADUAPI QString pwHash(const QString &text);
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);

KADUAPI void openWebBrowser(const QString &link);
KADUAPI void openMailClient(const QString &mail);
void openGGChat(const QString &gg);

QString versionToName(const unsigned int version);

QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString());

void KADUAPI saveWindowGeometry(const QWidget *w, const QString &section, const QString &name);
void KADUAPI loadWindowGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

QRect stringToRect(const QString &value, const QRect *def = NULL);
QString rectToString(const QRect& rect);

//usuwa znaki nowego wiersza, tagi htmla (wszystko co da si� dopasowa� jako <.*>)
QString toPlainText(const QString &text);

QList<int> toIntList(const QList<QVariant> &in);
QList<QVariant> toVariantList(const QList<int> &in);

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

#endif // MISC_H
