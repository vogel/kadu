/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "spellchecker.h"

#include "highlighter.h"

QList<Highlighter *> Highlighter::highlighters;
QTextCharFormat Highlighter::highlightFormat;

void Highlighter::rehighlightAll()
{
	foreach(Highlighter *highlighter, highlighters)
		highlighter->rehighlight();
}

Highlighter::Highlighter(QTextDocument* document) : QSyntaxHighlighter(document)
{
	highlighters.append(this);
}

Highlighter::~Highlighter()
{
	highlighters.remove(this);
}

void Highlighter::highlightBlock(const QString& text)
{
	QRegExp word("\\b\\w+\\b");

	int index = word.indexIn(text);
	while (index >= 0)
	{
		int length = word.matchedLength();
		if (!spellcheck->checkWord(word.cap()))
			setFormat(index, length, highlightFormat);
		index = word.indexIn(text, index + length);
	}
}

void Highlighter::setHighlightFormat(QTextCharFormat format)
{
	highlightFormat = format;
}

