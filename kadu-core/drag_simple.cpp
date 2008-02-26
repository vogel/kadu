/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <q3textstream.h>
#include <qtextcodec.h>

#include "drag_simple.h"

#include "debug.h"

DragSimple::DragSimple(const QString &mimeType, const QString &content, QWidget* dragSource, const char* name)
	: Q3TextDrag(dragSource, name), MimeType(mimeType), Content(content)
{
	kdebugf();
	kdebugf2();
}

const char * DragSimple::format(int i) const
{
	if (i == 0)
		return MimeType;
	else
		return 0;
}

bool DragSimple::provides(const char *mimeType) const
{
	return MimeType == mimeType;
}

QByteArray DragSimple::encodedData(const char *mimeType) const
{
	QByteArray result;

	if (!provides(mimeType))
		return result;

	Q3TextStream stream(result, QIODevice::WriteOnly);
	stream.setCodec(QTextCodec::codecForLocale());

	stream << Content;

	return result;
}
