/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
	highlighters.removeAll(this);
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

